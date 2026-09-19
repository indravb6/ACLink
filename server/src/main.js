const { setupHTTPServer } = require("./routes");
const { setupDB } = require("./db");

setupDB();
setupHTTPServer();
