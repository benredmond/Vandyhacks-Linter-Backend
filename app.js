const { exec } = require('child_process');
const parse = require('./parseAndMap');
const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const path = require('path');
const readImg = require('./gcp.js');
const multer = require("multer");
const fs = require("fs");

let storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, __dirname + '/images/')      //you tell where to upload the files,
    },
    filename: function (req, file, cb) {
        cb(null, file.fieldname + '-' + Date.now() + path.extname(file.originalname))
    }
});

let parser = multer({storage: storage});

// app.use(bodyParser.json({limit: '10mb', extended: true}));
// app.use(bodyParser.urlencoded({limit: '10mb', extended: true}));

// let lineToMessage = {};
// let typeToCount = {error: "0", warning: "0", style: "0"};
// function main(callback) {
//     exec('./cpp/Scripting main', (err, stdout, stderr) => {
//         exec('./cpp/compilecheck imgtext', (err, stdout, stderr) => {
//
//             if (err) {
//                 // node couldn't execute the command
//                 console.log("BAD");
//                 return;
//             }
//
//             // if (stdout.toString().contains("EXIT CODE: 0")) {
//             //     console.log("successful");
//             // }
//
//             console.log(`stdout: ${stdout}`);
//             console.log(`stderr: ${stderr}`);
//             parse.parse(lineToMessage, typeToCount, stdout);
//             console.log(lineToMessage);
//             console.log (typeToCount);
//             callback(stdout);
//
//         });
//     });
// }
//
// main(() => {
//     exec('./cpp/main.out', (err, stdout, stderr) => {
//         console.log(`stdout: ${stdout}`);
//         console.log(`stderr: ${stderr}`);
//     });
// });

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/index.html');
});

app.post('/upload', parser.single("image"), function(req, res, next) {
    res.redirect('/');
    readImg("./images/" + req.file.filename, (imgText) => {
        let content = '#include <iostream>\nint main() {\n' + imgText + 'return 0;\n}';
        fs.writeFile('./cpp/imgtext.cpp', content, () => {
            console.log(imgText);
        });
    });
});

app.listen(3000, () => {
    console.log('We are live on ' + 3000);
});




