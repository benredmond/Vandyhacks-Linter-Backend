const {exec} = require('child_process');
const parse = require('./parseAndMap');
const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const path = require('path');
const readImg = require('./gcp.js');
const multer = require("multer");
const fs = require("fs");
const alg = require('./scoreAlgorithm');

app.use(bodyParser.json({limit: '10mb', extended: true}));
app.use(bodyParser.urlencoded({limit: '10mb', extended: true}));

let storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, __dirname + '/images/')      //you tell where to upload the files,
    },
    filename: function (req, file, cb) {
        cb(null, file.fieldname + '-' + Date.now() + path.extname(file.originalname))
    }
});

let parser = multer({storage: storage});

let lineToMessage = {};
let typeToCount = {error: 0, warning: 0, style: 0};

function main(callback) {
    let fileName = "main";
    performAnalysis(fileName, lineToMessage, typeToCount);
}

function performAnalysis (fileName, lineToMessage, typeToCount){
    exec(`./cpp/CompileCheck ${fileName}`, (err, stdout, stderr) => {

        if (err) {
            // node couldn't execute the command
            console.log("Error when reading the file!");
            return;
        }

        let errorCode = stdout;
        console.log("YOUR CODE FINISHED WITH " + stdout);
        if (errorCode.includes("EXIT CODE: 0")) {
            exec('./cpp/Scripting main', (err, stdout, stderr) => {
                console.log("\n" + "Grading...");
                console.log(stdout);
                console.log("the errors maps read:");
                parse.parse(lineToMessage, typeToCount, stdout);
                console.log("line to message mapping:");
                console.log(lineToMessage);
                console.log("type to count mapping: ");
                console.log(typeToCount);


                console.log("\nYour program output: \n");
                exec('./cpp/main.out', (err, stdout, stderr) => {
                    console.log(`stdout: ${stdout}`);
                })
                var lineToMessageJSON = JSON.stringify(lineToMessage);
                var typeToCountJSON = JSON.stringify(typeToCount);
                var mergedReport = {lineToMessage:lineToMessageJSON, typeToCount:typeToCountJSON};
                var mergedJSONreport = JSON.stringify(mergedReport);
                return mergedJSONreport;

            })
        }
        else {
            console.log("Process must finish exit code 0 before receiving a grade!");
        }
    });
}

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/index.html');
});

app.post('/upload', parser.single("image"), function(req, res, next) {
    res.redirect('/');
    readImg("./images/" + req.file.filename, (imgText) => {
        let content = '#include <iostream>\nint main() {\n' + imgText + 'return 0;\n}';
        fs.writeFile('./cpp/outputfile.cpp', content, () => {
            performAnalysis('outputfile');
        });
    });
});

app.post('/parseText', (req, res) => {
    res.redirect('/');
    let content = '#include <iostream>\nint main() {\n' + req.body.comment + 'return 0;\n}';
    fs.writeFile('./cpp/outputfile.cpp', content, () => {
        performAnalysis('outputfile');
    });
});

app.listen(3000, () => {
    console.log('We are live on ' + 3000);
});
