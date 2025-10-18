const express = require('express');
const { execFile } = require('child_process');
const app = express();
app.use(express.json());

app.post('/transaction', (req, res) => {
  const { accNo, amount, location } = req.body;

  execFile('./fraudBackend', [accNo, amount, location], (err, stdout, stderr) => {
    if (err) return res.status(500).json({ error: stderr });
    res.setHeader('Content-Type', 'application/json');
    res.send(stdout);
  });
});

app.use(express.static('public'));

app.listen(3000, () => console.log('Server running: http://localhost:3000'));
