// routes/loan.js

const express = require('express');
const router = express.Router();
const db = require('../db/db');

// 도서 대출 처리 API (POST /loans)
router.post('/', (req, res) => {
    // 1. 요청 본문에서 UserID, ItemID를 받음
    // 2. 해당 Item이 대출 가능한 상태인지 확인 (Book_item 테이블)
    // 3. User의 최대 대출 권수 확인 (User 테이블)
    // 4. Loan 테이블에 기록 추가 (LoanDate, DueDate 계산)
    // 5. Book_item의 status를 '대출 중'으로 업데이트
    res.send('Book loan processed');
});

// 도서 반납 처리 API (POST /loans/return/:itemID)
router.post('/return/:itemID', (req, res) => {
    // 1. ItemID로 Loan 기록 검색 및 ReturnDate, OverdueDays 계산
    // 2. Loan 테이블 업데이트
    // 3. Book_item의 status를 '대출 가능'으로 업데이트
    // 4. 연체 발생 시 User 테이블의 solveDate와 status 업데이트
    res.send(`Book return processed for Item ID: ${req.params.itemID}`);
});

// 특정 사용자의 대출 목록 조회 API (GET /loans/user/:userID)
router.get('/user/:userID', (req, res) => {
    // 1. UserID로 Loan, Book_item, Book 테이블을 JOIN하여 대출 목록 조회
    res.send(`Loan list for User ID: ${req.params.userID}`);
});

module.exports = router;