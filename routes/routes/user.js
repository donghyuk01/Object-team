const express = require('express');
const router = express.Router();
const db = require('../db/db'); // 필요 시 주석 해제

// 사용자 등록 (예: 회원가입) API
router.post('/', (req, res) => {
    // 회원가입 로직 구현
    res.send('User registration endpoint');
});

// 사용자 정보 조회 API
router.get('/:userID', (req, res) => {
    // 사용자 정보 조회 로직 구현
    res.send(`User info for ID: ${req.params.userID}`);
});

module.exports = router;