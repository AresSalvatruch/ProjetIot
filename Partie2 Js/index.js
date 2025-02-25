import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';

// Initialize the serial port (COM7 for your case)
const port = new SerialPort({ path: 'COM7', baudRate: 9600 }, function (err) {
  if (err) {
    console.log('Error:', err.message);
  } else {
    console.log('Port série ouvert sur COM7');
  }
});

// Use the ReadlineParser to handle incoming serial data line by line
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

// Listen for incoming data
parser.on('data', (data) => {
  // Print the data received from the Arduino (i.e., the luminosity value)
  console.log('Luminosité reçue:', data);
});

// Handle port errors
port.on('error', function(err) {
  console.log('Erreur du port série:', err.message);
});