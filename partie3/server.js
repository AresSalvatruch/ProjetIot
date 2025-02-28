const express = require('express');
const mqtt = require('mqtt');
const http = require('http');
const socketIo = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

// MQTT Client
const mqttClient = mqtt.connect('mqtt://localhost:1883');
let arduinoData = {};

// Subscribe to Arduino status topic
mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
  mqttClient.subscribe('alarm/system/status', (err) => {
    if (err) console.error('Failed to subscribe');
    else console.log('Subscribed to alarm/system/status');
  });
});

// Handle incoming MQTT messages
mqttClient.on('message', (topic, message) => {
  if (topic === 'alarm/system/status') {
    try {
      arduinoData = JSON.parse(message.toString());
    } catch (error) {
      arduinoData = { rawMessage: message.toString() };
    }
    io.emit('update', arduinoData);
  }
});

// Stop alarm route
app.post('/stop_alarm', (req, res) => {
  console.log("Stopping alarm...");
  mqttClient.publish('alarm/system/control', JSON.stringify({ command: "STOP_ALARM" }));
  res.json({ success: true, message: "Alarm stop command sent" });
});

// Serve static files
app.use(express.static('public'));

// Start server
const PORT = 3000;
server.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
