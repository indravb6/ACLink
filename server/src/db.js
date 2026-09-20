const path = require("path");
const fs = require("fs");
const { v4: uuidv4 } = require("uuid");

const file = path.join(__dirname, "..", "data.json");

const readDB = () => {
  return JSON.parse(fs.readFileSync(file, "utf-8"));
};

const writeDB = (data) => {
  fs.writeFileSync(file, JSON.stringify(data, null, 2));
};

const setupDB = () => {
  if (!fs.existsSync(file)) {
    const initialData = {
      currentCondition: {
        temperature: -1,
        humidity: -1,
      },
      settings: {
        id: uuidv4(),
        on: true,
        targetTemperature: 22,
        mode: "cool",
        fanSpeed: "auto",
        swing: "1-5",
      },
    };

    writeDB(initialData);
  }
};

module.exports = { readDB, writeDB, setupDB };
