const express = require('express');
const db = require('./db/db'); 
const bookRouter = require('./routes/book'); 
const path = require('path');

const app = express();
const port = 8080;
app.use(express.json({ limit: '10mb' }));
app.use(express.json());

app.use('/public', express.static(path.join(__dirname, 'public')));
app.use(express.static('public')); 

app.get('/', (req, res) => {
    res.send('Node.js API 서버가 정상 작동 중입니다.');
});


app.use('/book', bookRouter); 


app.listen(port, () => {
    console.log(`Express 서버가 http://localhost:${port} 에서 실행 중입니다.`);
});