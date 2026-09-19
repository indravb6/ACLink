const http = require("http");
const fs = require("fs");
const path = require("path");

const PORT = 7000;

let acState = false;

const server = http.createServer((req, res) => {
  // Serve index.html
  if (req.method === "GET" && req.url === "/") {
    const html = fs.readFileSync(path.join(__dirname, "index.html"), "utf-8");

    res.writeHead(200, {
      "Content-Type": "text/html",
    });

    res.end(html);
    return;
  }

  // ESP32: get AC state
  if (req.method === "GET" && req.url === "/api/ac/status") {
    res.writeHead(200, {
      "Content-Type": "text/plain",
      "Cache-Control": "no-store",
    });

    res.end(acState ? "1" : "0");
    return;
  }

  // Web: toggle AC
  if (req.method === "POST" && req.url === "/api/ac/toggle") {
    acState = !acState;

    console.log(`AC: ${acState ? "ON" : "OFF"}`);

    res.writeHead(200, {
      "Content-Type": "application/json",
      "Cache-Control": "no-store",
    });

    res.end(
      JSON.stringify({
        on: acState,
      }),
    );

    return;
  }

  res.writeHead(404);
  res.end("Not Found");
});

server.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});
