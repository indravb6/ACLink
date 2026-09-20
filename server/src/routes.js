const express = require("express");
const path = require("path");
const { readDB, writeDB } = require("./db");
const { v4: uuidv4 } = require("uuid");
const app = express();
const port = 7000;

app.use(express.json());

const setupHTTPServer = () => {
  app.get("/api/state", (req, res) => {
    const data = readDB();
    res.json(data);
  });

  app.post("/api/state", (req, res) => {
    const data = readDB();
    data.settings = req.body;
    data.settings.id = uuidv4();
    writeDB(data);
    res.json(data);
  });

  app.use(express.static(path.join(__dirname, "..", "web")));

  app.listen(port, () => {
    console.log(`app listening on http://localhost:${port}`);
  });
};

module.exports = { setupHTTPServer };
