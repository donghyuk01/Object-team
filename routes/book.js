const express = require('express');
const db = require('../db/db');
const router = express.Router();
const fs = require('fs');
const path = require('path');

// UPLOAD_DIR 경로를 'public' 폴더 자체로 변경
const UPLOAD_DIR = path.join(__dirname, '..', 'public');

// POST / 요청 처리: JSON 본문만 받습니다.
router.post('/', (req, res) => {
    
    console.log('✅ POST / 요청 (JSON) 성공적으로 수신됨');
    const { title, author, category, publisher, isbc, imageBase64 } = req.body;
    
    // 필수 필드 확인
    if (!title || !author || !category || !imageBase64) {
        console.error('필수 필드 (제목, 저자, 카테고리, 이미지) 누락');
        return res.status(400).send('필수 필드 (제목, 저자, 카테고리, 이미지) 누락');
    }

    let imagePath = null;
    let filePath = null;

    try {
        // 1. Base64 데이터 준비 (헤더 제거)
        const base64Data = imageBase64.replace(/^data:image\/\w+;base64,/, "");
        const imageBuffer = Buffer.from(base64Data, 'base64');

        // 2. 파일 이름 및 경로 설정
        const fileExt = '.png';
        const fileName = `${Date.now()}-${title.slice(0, 10)}${fileExt}`;
        
        // filePath는 public 폴더 바로 아래 파일로 지정됨
        filePath = path.join(UPLOAD_DIR, fileName);

        // 3. 파일 시스템에 저장
        fs.writeFileSync(filePath, imageBuffer);
        
        // 4. DB에 저장할 웹 접근 경로 설정 (public 폴더 루트)
        // server.js에서 public을 정적 파일로 설정했기 때문에 바로 /파일명 으로 접근 가능
        imagePath = `/public/${fileName}`;
        console.log(`🎉 파일 저장 성공: ${imagePath}`);

    } catch (e) {
        console.error('❌ Base64 처리 또는 파일 저장 오류:', e);
        // 오류 발생 시 파일 시스템에 잘못된 데이터가 저장될 위험이 없어 삭제 로직은 생략
        return res.status(500).send('이미지 파일 처리 중 오류가 발생했습니다.');
    }

    const finalPublisher = publisher || null;
    const finalIsbc = isbc || null;

    // SQL 쿼리: imagePath 컬럼 사용
    const sql = `INSERT INTO Book (title, author, category, publisher, isbc, imagePath) 
                 VALUES (?, ?, ?, ?, ?, ?)`;
    
    const values = [
        title, 
        author, 
        category, 
        finalPublisher, 
        finalIsbc, 
        imagePath
    ];

    db.query(sql, values, (err, result) => {
        if (err) {
            console.error('❌ DB INSERT 오류:', err);
            // DB INSERT 실패 시 저장된 파일 삭제
            if (filePath) {
                try {
                    fs.unlinkSync(filePath);
                    console.log(`⚠️ DB 오류로 인해 저장된 파일 삭제됨: ${filePath}`);
                } catch(unlinkErr) {
                    console.error("파일 삭제 오류:", unlinkErr);
                }
            }
            return res.status(500).send(`DB 오류 발생: ${err.sqlMessage}`);
        }

        console.log('📌 INSERT 성공:', result.insertId);
        res.status(201).send({ message: '도서 등록 성공', id: result.insertId, imagePath });
    });
});
router.get('/search', (req, res) => {
    const { type, keyword, sort, category } = req.query; 

    let searchField = type === 'author' ? 'author' : 'title';
    let sqlCondition = '';
    let sqlValues = [];

    console.log(`🔍 검색 요청: 유형=${type}, 키워드=${keyword}, 정렬=${sort}, 카테고리=${category}`);
	
    // 1. 키워드 유무에 따른 WHERE 절 결정
    if (keyword && keyword.trim() !== "") {
        sqlCondition = `WHERE b.${searchField} LIKE ?`;
        sqlValues.push(`%${keyword}%`);
    }

    // 2. 카테고리 필터 조건 추가
    if (category && category.trim() !== "") {
        sqlCondition += sqlCondition === '' ? ' WHERE ' : ' AND ';
        sqlCondition += 'b.category = ?';
        sqlValues.push(category); 
    }
    
    // 3. 정렬 조건 설정
    let orderByClause = 'b.bookID DESC'; // 기본값 (최신순)
    if (sort === 'titleAsc') {
        orderByClause = 'b.title ASC'; // 제목순
    } else if (sort === 'regDateDesc') {
        orderByClause = 'b.regDate DESC'; // 최신순
    }
    
    // 4. SQL 쿼리 구성 (주석 제거)
    const sql = `
        SELECT 
            b.bookID, b.title, b.author, b.publisher, b.imagePath, b.category,
            (SELECT COUNT(*) FROM Book_item bi WHERE bi.bookID = b.bookID AND bi.status = '대출 가능') as available_count
        FROM Book b
        ${sqlCondition}
        ORDER BY ${orderByClause}
    `;
    
    // 5. 쿼리 실행
    db.query(sql, sqlValues, (err, results) => { 
        if (err) {
            console.error('❌ 검색/정렬 쿼리 오류:', err);
            return res.status(500).send('DB 오류');
        }

        const responseData = results.map(row => ({
            title: row.title,
            author: row.author,
            publisher: row.publisher,
            imagePath: row.imagePath,
            status: row.available_count > 0 ? '대출 가능' : '대출 불가' 
        }));

        res.json(responseData);
    });
});
module.exports = router;