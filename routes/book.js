const express = require('express');
const db = require('../db/db');
const router = express.Router();
const fs = require('fs');
const path = require('path');

// UPLOAD_DIR 경로를 'public' 폴더 자체로 변경
const UPLOAD_DIR = path.join(__dirname, '..', 'public');

// -----------------------------------------------------------------
// 1. 도서 등록 (POST /)
// -----------------------------------------------------------------
router.post('/', async (req, res) => {
    console.log('✅ POST / 요청 (JSON) 성공적으로 수신됨');

    const { title, author, category, publisher, isbc, imageBase64 } = req.body;

    // 필수 필드 확인
    if (!title || !author || !category || !imageBase64) {
        console.error('필수 필드 (제목, 저자, 카테고리, 이미지) 누락');
        return res.status(400).send('필수 필드 (제목, 저자, 카테고리, 이미지) 누락');
    }

    const finalPublisher = publisher || null;
    const finalIsbc = isbc || null;
    let imagePath = null;
    let filePath = null;
    let bookID = null;
    let shouldInsertNewBook = false;

    // 1. 기존 도서 확인 (제목, 저자, 출판사가 모두 일치하는지 확인)
    const checkSql = `
        SELECT bookID
        FROM Book
        WHERE title = ? AND author = ? AND publisher ${finalPublisher === null ? 'IS NULL' : '= ?'}
    `;
    const checkValues = [title, author];
    if (finalPublisher !== null) {
        checkValues.push(finalPublisher);
    }

    try {
        const [existingBooks] = await db.promise().query(checkSql, checkValues);

        if (existingBooks.length > 0) {
            // 🚨 1. 이미 존재하는 책: 기존 bookID 사용
            bookID = existingBooks[0].bookID;
            shouldInsertNewBook = false;
            // ... (로그 생략) ...
        } else {
            // ✅ 2. 새로운 책: DB에 먼저 Book 등록 필요, 이미지 저장 필수
            shouldInsertNewBook = true;
            console.log('🎉 새로운 도서. Book 및 Book_item 등록이 필요합니다.');

            // 2-1. 이미지 파일 처리 (새로운 책 등록 시에만 실행)
            const base64Data = imageBase64.replace(/^data:image\/\w+;base64,/, '');
            const imageBuffer = Buffer.from(base64Data, 'base64');

            const fileExt = '.png';
            const fileName = `${Date.now()}-${title.slice(0, 10)}${fileExt}`;
            filePath = path.join(UPLOAD_DIR, fileName);

            fs.writeFileSync(filePath, imageBuffer); // 파일 시스템에 저장
            imagePath = `/${fileName}`; // DB에 저장할 웹 접근 경로
            console.log(`🎉 파일 저장 성공: ${fileName}, DB 경로: ${imagePath}`);

            // 2-2. Book 테이블에 등록 (⭐ 이 부분이 누락되었었습니다) ⭐
            const insertBookSql = `
                INSERT INTO Book
                    (title, author, category, publisher, isbc, imagePath)
                VALUES
                    (?, ?, ?, ?, ?, ?)
            `;
            const insertBookValues = [
                title,
                author,
                category,
                finalPublisher,
                finalIsbc,
                imagePath
            ];
            const [bookResult] = await db.promise().query(insertBookSql, insertBookValues);
            bookID = bookResult.insertId;
        }

        // ⭐ 3. Book_item 테이블에 새 항목 추가 (공통 로직) ⭐
        
        // 3-1. 현재 bookID에 해당하는 item 개수 카운트
        const countSql = `
            SELECT COUNT(itemID) AS item_count
            FROM Book_item
            WHERE bookID = ?
        `;
        const [countResult] = await db.promise().query(countSql, [bookID]);
        const currentCount = countResult[0].item_count;
        
        // 3-2. 새 item의 번호 (기존 개수 + 1)를 4자리 숫자로 포맷팅
        const newItemNumber = currentCount + 1;
        const itemNumberFormatted = String(newItemNumber).padStart(4, '0');

        // 3-3. ItemID 생성: [bookID]-[count]
        const itemID = `${bookID}-${itemNumberFormatted}`; 
        
        // 3-4. Book_item 등록
        const insertItemSql = `
            INSERT INTO Book_item (itemID, bookID, status, aDate)
            VALUES (?, ?, '대출 가능', CURDATE())
        `;
        const insertItemValues = [itemID, bookID];
        await db.promise().query(insertItemSql, insertItemValues);

        console.log(`📌 Book_item 등록 성공: itemID ${itemID}, bookID ${bookID}`);


        res.status(201).send({
            message: shouldInsertNewBook ? '새 도서 및 아이템 등록 성공' : '기존 도서에 새 아이템 추가 성공',
            id: bookID,
            itemID: itemID,
            imagePath: imagePath // 새로운 책일 경우에만 imagePath가 담김
        });
    } catch (e) {
        console.error('❌ 도서 등록 처리 중 오류:', e);

        // 오류 발생 시 파일 시스템에 저장된 이미지 삭제 (새로운 책일 경우에만 해당)
        if (filePath && shouldInsertNewBook) {
            try {
                fs.unlinkSync(filePath);
                console.log(`⚠️ DB 오류로 파일 삭제됨: ${filePath}`);
            } catch (unlinkErr) {
                console.error('파일 삭제 오류:', unlinkErr);
            }
        }

        return res
            .status(500)
            .send(`서버 오류 발생: ${e.message}`);
    }
});

// -----------------------------------------------------------------
// 2. 도서 목록/검색 (GET /search) - 와일드카드보다 위에 배치해야 함
// -----------------------------------------------------------------
router.get('/search', (req, res) => {
    const { type, keyword, sort, category } = req.query;

    const searchField = type === 'author' ? 'author' : 'title';
    let sqlCondition = '';
    const sqlValues = [];

    console.log(
        `🔍 검색 요청: 유형=${type}, 키워드=${keyword}, 정렬=${sort}, 카테고리=${category}`
    );

    // 키워드 조건
    if (keyword && keyword.trim() !== '') {
        sqlCondition = `WHERE b.${searchField} LIKE ?`;
        sqlValues.push(`%${keyword}%`);
    }

    // 카테고리 필터
    if (category && category.trim() !== '') {
        sqlCondition += sqlCondition === '' ? ' WHERE ' : ' AND ';
        sqlCondition += 'b.category = ?';
        sqlValues.push(category);
    }

    // 정렬 조건
    let orderByClause = 'b.bookID DESC';
    if (sort === 'titleAsc') {
        orderByClause = 'b.title ASC';
    } else if (sort === 'regDateDesc') {
        orderByClause = 'b.regDate DESC';
    }

    const sql = `
    SELECT
        b.bookID,
        b.title,
        b.author,
        b.publisher,
        b.imagePath,
        b.category,

        COUNT(bi.itemID) AS total_count,
        SUM(bi.status = '대출 가능') AS available_count,

        CASE
            WHEN SUM(bi.status = '대출 가능') > 0 THEN '대출 가능'
            ELSE '대출 불가'
        END AS status

    FROM Book b
    LEFT JOIN Book_item bi
        ON b.bookID = bi.bookID

    ${sqlCondition}

    GROUP BY b.bookID
    ORDER BY ${orderByClause}
`;

    db.query(sql, sqlValues, (err, results) => {
        if (err) {
            console.error('❌ 검색/정렬 쿼리 오류:', err);
            return res.status(500).send('DB 오류');
        }

        const responseData = results.map(row => ({
            bookID: row.bookID, // 클라이언트 리스트 뷰에서 ItemData로 사용해야 하므로 추가
            title: row.title,
            author: row.author,
            publisher: row.publisher,
            imagePath: row.imagePath,
            category: row.category,
            status: row.status,                 // ✅ SQL에서 만든 값
            totalCount: row.total_count,        // ✅ 전체 권수
            availableCount: row.available_count
        }));

        res.json(responseData);
    });
});

// -----------------------------------------------------------------
// 3. 도서 상세 조회 (GET /:bookId) - 와일드카드이므로 뒤에 배치
// -----------------------------------------------------------------
router.get('/:bookId', (req, res) => {
    const { bookId } = req.params;

    const sql = `
        SELECT bookID, title, author, category, publisher, imagePath, isbc
        FROM Book
        WHERE bookID = ?
    `;

    db.query(sql, [bookId], (err, results) => {
        if (err) {
            console.error('❌ 상세 정보 조회 쿼리 오류:', err);
            return res.status(500).send('DB 오류');
        }

        if (results.length === 0) {
            console.log(`⚠️ 도서 ID ${bookId}를 찾을 수 없음`);
            return res.status(404).send('해당 ID의 도서를 찾을 수 없습니다.');
        }

        // 도서 정보를 JSON 객체로 반환
        const bookData = results[0];
        res.json({
            bookID: bookData.bookID,
            title: bookData.title,
            author: bookData.author,
            category: bookData.category,
            publisher: bookData.publisher,
            imagePath: bookData.imagePath,
            isbc: bookData.isbc
        });

        console.log(`📌 상세 정보 조회 성공: BookID ${bookId}`);
    });
});

// -----------------------------------------------------------------
// 4. 도서 수정 (PUT /:bookId)
// -----------------------------------------------------------------
router.put('/:bookId', (req, res) => {
    const { bookId } = req.params;
    const { title, author, category, publisher, isbc } = req.body;

    // TODO: 이미지 수정 로직은 복잡하므로, 현재는 텍스트 필드만 수정하도록 처리합니다.

    // 필수 필드 확인
    if (!title || !author || !category) {
        return res.status(400).send('제목, 저자, 카테고리는 필수 입력 사항입니다.');
    }

    const finalPublisher = publisher || null;
    const finalIsbc = isbc || null;

    const sql = `
        UPDATE Book
        SET 
            title = ?, 
            author = ?, 
            category = ?, 
            publisher = ?, 
            isbc = ?
        WHERE bookID = ?
    `;
    const values = [title, author, category, finalPublisher, finalIsbc, bookId];

    db.query(sql, values, (err, result) => {
        if (err) {
            console.error('❌ DB UPDATE 오류:', err);
            return res.status(500).send(`DB 오류 발생: ${err.sqlMessage}`);
        }

        if (result.affectedRows === 0) {
            return res.status(404).send('해당 ID의 도서를 찾을 수 없습니다.');
        }

        console.log(`📌 UPDATE 성공: BookID ${bookId}`);
        res.status(200).send({ message: '도서 정보 수정 성공' });
    });
});

// -----------------------------------------------------------------
// 5. 도서 삭제 (DELETE /:bookId)
// -----------------------------------------------------------------
router.delete('/:bookId', (req, res) => {
    const { bookId } = req.params;

    // TODO: 이미지 파일도 삭제해야 하지만, 현재는 DB 레코드만 삭제합니다.

    const sql = 'DELETE FROM Book WHERE bookID = ?';

    db.query(sql, [bookId], (err, result) => {
        if (err) {
            console.error('❌ DB DELETE 오류:', err);
            // 외래 키 제약 조건 위반 오류 처리 (Book_item에 연결된 경우)
            if (err.code === 'ER_ROW_IS_REFERENCED_2') {
                return res.status(409).send('대출된 항목이 있어 도서를 삭제할 수 없습니다. 먼저 대출 목록에서 항목을 정리해 주세요.');
            }
            return res.status(500).send(`DB 오류 발생: ${err.sqlMessage}`);
        }

        if (result.affectedRows === 0) {
            return res.status(404).send('해당 ID의 도서를 찾을 수 없습니다.');
        }

        console.log(`📌 DELETE 성공: BookID ${bookId}`);
        res.status(200).send({ message: '도서 삭제 성공' });
    });
});

module.exports = router;