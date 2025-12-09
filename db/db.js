const mysql = require("mysql2");

const db = mysql.createConnection({
  host: "155.230.235.248",
  port: 32065,
  user: "mir9876",
  password: "rlaehdgur",
  database: "mir9876",
  charset: 'utf8mb4'
});

db.connect((err) => {
  if (err) return console.error("DB 연결 실패:", err);
  console.log("MySQL 연결 성공!");
});

module.exports = db;
