#pragma once

const char PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>E-paper</title>
</head>
<body>
  <h3>E-paper</h3>
  <input id="f" type="file" accept=".jpg,.jpeg,image/jpeg">
  <br><br>
  <button type="button" onclick="send()">Show</button>
  <button type="button" onclick="clr()">Clear</button>
  <p id="s"></p>
<script>
async function send() {
  const el = document.getElementById("f");
  const file = el.files && el.files[0];
  const s = document.getElementById("s");
  if (!file) { s.textContent = "pick a file first"; return; }

  s.textContent = "resizing...";
  const img = await createImageBitmap(file);
  const canvas = document.createElement("canvas");
  canvas.width = 240;
  canvas.height = 240;
  const ctx = canvas.getContext("2d");
  ctx.fillStyle = "#fff";
  ctx.fillRect(0, 0, 240, 240);
  const scale = Math.min(240 / img.width, 240 / img.height);
  const w = img.width * scale, h = img.height * scale;
  ctx.drawImage(img, (240 - w) / 2, (240 - h) / 2, w, h);

  const blob = await new Promise(res => canvas.toBlob(res, "image/jpeg", 0.85));
  s.textContent = "uploading " + blob.size + " bytes";

  const fd = new FormData();
  fd.append("file", blob, "photo.jpg");
  try {
    const r = await fetch("/upload", { method: "POST", body: fd });
    s.textContent = await r.text();
  } catch (e) {
    s.textContent = "failed: " + e;
  }
}
async function clr() {
  try {
    const r = await fetch("/clear", { method: "GET" });
    s.textContent = await r.text();
  } catch (e) {
    s.textContent = "failed: " + e;
  }
}
</script>
</body>
</html>
)HTML";