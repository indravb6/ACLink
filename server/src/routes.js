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

  app.post("/app/temp/:temperature/:humidity", (req, res) => {
    const temperature = Number(req.params.temperature);
    const humidity = Number(req.params.humidity);

    if (Number.isNaN(temperature) || Number.isNaN(humidity)) {
      return res.status(400).json({
        error: "Invalid temperature or humidity",
      });
    }

    const data = readDB();

    data.currentCondition.temperature = temperature;
    data.currentCondition.humidity = humidity;

    writeDB(data);

    res.json(data.currentCondition);
  });

  app.use(express.static(path.join(__dirname, "..", "web")));

  app.listen(port, () => {
    console.log(`app listening on http://localhost:${port}`);
  });
};

module.exports = { setupHTTPServer };
