// servient.js

const mqtt = require('mqtt');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

// MQTT setup
const mqttClient = mqtt.connect('mqtt://localhost'); // Replace with your MQTT broker URL
const mqttTopic = 'alarm/system/status'; // Topic for receiving Arduino data
const controlTopic = 'alarm/system/control'; // Topic for control commands

// Serial port setup
const serialPort = new SerialPort({
  path: 'COM3', // Adjust based on your system (e.g., COM3 for Windows)
  baudRate: 9600
});

const parser = serialPort.pipe(new ReadlineParser({ delimiter: '\n' }));

// Connect to MQTT broker
mqttClient.on('connect', () => {
  console.log('Connected to MQTT broker');
  mqttClient.subscribe(mqttTopic, (err) => {
    if (err) {
      console.error('Failed to subscribe to topic:', err);
    } else {
      console.log('Subscribed to topic:', mqttTopic);
    }
  });

  mqttClient.subscribe(controlTopic, (err) => {
    if (err) {
      console.error('Failed to subscribe to control topic:', err);
    } else {
      console.log('Subscribed to control topic:', controlTopic);
    }
  });
});

// Handle incoming MQTT messages
mqttClient.on('message', (topic, message) => {
  console.log(`Received message on topic ${topic}:`, message.toString());

  // Handle the status topic
  if (topic === mqttTopic) {
    try {
      // Try to parse as JSON
      const jsonData = JSON.parse(message);
      console.log('Received JSON:', jsonData);

      // You can handle the JSON data here
      // For example, send data to the Arduino via serial if needed
      serialPort.write(`{"command": "status", "data": ${JSON.stringify(jsonData)}}\n`);
    } catch (error) {
      // If not JSON, handle it as non-JSON
      console.log('Non-JSON message:', message.toString());
      serialPort.write(message.toString() + '\n');
    }
  }

  // Handle the control topic
  if (topic === controlTopic) {
    try {
      const commandData = JSON.parse(message.toString());
      if (commandData.action === 'stop') {
        console.log('Stopping the alarm...');
        
        // Send the stop command to Arduino via serial
        serialPort.write('{"command": "stop_alarm"}\n');  // Change this based on Arduino's expected command
      }
    } catch (error) {
      console.error('Error parsing control message:', error);
    }
  }
});

// Handle serial data from Arduino
parser.on('data', (data) => {
  console.log('Received from Arduino:', data);

  try {
    // Try to parse as JSON
    const jsonData = JSON.parse(data);
    console.log('Arduino JSON:', jsonData);

    // Process the received JSON (for example, send it to MQTT broker)
    mqttClient.publish(mqttTopic, JSON.stringify(jsonData));
  } catch (error) {
    // If not JSON, just print the data
    console.log('Non-JSON data received from Arduino:', data);
  }
});

// Error handling for the serial port
serialPort.on('error', (err) => {
  console.error('Serial port error:', err.message);
});

// Exit cleanly on process termination
process.on('SIGINT', () => {
  mqttClient.end();
  serialPort.close(() => {
    console.log('Closed serial port');
    process.exit();
  });
});
