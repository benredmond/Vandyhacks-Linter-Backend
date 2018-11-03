const {exec} = require('child_process');
const parse = require('./parseAndMap');

let lineToMessage = {};
let typeToCount = {error: 0, warning: 0, style: 0};

function main(callback) {
    let fileName = "main";
    performAnalysis(fileName);
}

main(() => {
    exec('./cpp/main.out', (err, stdout, stderr) => {
        console.log(`stdout: ${stdout}`);
        //console.log(`stderr: ${stderr}`);
    });
});


function performAnalysis (fileName){
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
                    //console.log(`stderr: ${stderr}`);
                });
            })
        }
        else {
            console.log("Process must finish exit code 0 before receiving a grade!");
        }
    });
}



