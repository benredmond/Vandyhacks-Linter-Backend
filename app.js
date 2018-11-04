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
const compile = require('./compile_api');


app.use(bodyParser.json({limit: '10mb', extended: true}));
app.use(bodyParser.urlencoded({limit: '10mb', extended: true}));

app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

let storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, __dirname + '/images/')      //you tell where to upload the files,
    },
    filename: function (req, file, cb) {
        cb(null, file.fieldname + '-' + Date.now() + path.extname(file.originalname))
    }
});

let parser = multer({storage: storage});

function performAnalysis (fileName, code, callback){
    let lineToMessage = {};
    let typeToCount = {error: 0, warning: 0, style: 0};

    compile(code, (compileResults) => {
        compileResults = JSON.parse(compileResults);
        if (compileResults["status"] !== "completed") {
            console.log("Error when reading the file!");
            callback(null);
        }

        console.log("YOUR CODE FINISHED WITH ERROR CODE " + compileResults["build_exit_code"]);
        if (compileResults["build_exit_code"] === 0) {
            exec(`./cpp/Scripting ${fileName}`, (err, stdout, stderr) => {
                console.log("\n" + "Grading...");
                console.log(stdout);
                console.log("the errors maps read:");
                parse.parse(lineToMessage, typeToCount, stdout);
                console.log("line to message mapping:");
                console.log(lineToMessage);
                console.log("type to count mapping: ");
                console.log(typeToCount);

                console.log("\nYour program output: ");
                let mergedReport = {
                    lineToMessage,
                    typeToCount,
                    output: compileResults["stdout"]
                };
                console.log("mergedReport looks like this:");
                console.log(mergedReport);
                
                callback(mergedReport);
                return mergedReport;
            })
        }
        else {
            console.log("Process must finish exit code 0 before receiving a grade!");
            callback(null);
        }
    });
}

app.get('/', (req, res) => {
    res.sendFile(__dirname + '/index2.html');
});

app.post('/upload', parser.single("image"), function(req, res, next) {

    console.log(req);
    readImg("./images/" + req.file.filename, (imgText) => {
        let content = '#include <iostream>\nint main() {\n' + imgText + 'return 0;\n}';
        fs.writeFile('./cpp/outputfile.cpp', content, () => {
            performAnalysis('outputfile', content, (output) => {
                if (output !== null) {
                    alg.calculateScore(output.typeToCount, (score) => {
                        output["score"] = score;
                        res.send(output);
                    });
                } else {
                    res.send('bad');
                }
            });
        });
    });
});

app.post('/parseText', (req, res) => {
    // res.redirect('/');
    let content = '#include <iostream>\nint main() {\n' + req.body.comment + 'return 0;\n}';
    fs.writeFile('./cpp/outputfile.cpp', content, () => {
        performAnalysis('outputfile', content, (output) => {
            if (output !== null) {
                // res = JSON.parse(res);
                 alg.calculateScore(output.typeToCount, (score) => {
                     output["score"] = score;
                     res.send(output);
                     // res.redirect('/');
                 });
                // console.log(res);
            } else {
                res.send('bad');
            }
        });
    });
});

app.listen(process.env.PORT || 3000);