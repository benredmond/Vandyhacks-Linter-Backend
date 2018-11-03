const { exec } = require('child_process');
const parse = require('./parseAndMap');

let lineToMessage = {};
let typeToCount = {error: "0", warning: "0", style: "0"};
function main(callback) {
    exec('./cpp/Scripting main', (err, stdout, stderr) => {
        if (err) {
            // node couldn't execute the command
            console.log("BAD");
            return;
        }

        // if (stdout.toString().contains("EXIT CODE: 0")) {
        //     console.log("successful");
        // }

        console.log(`stdout: ${stdout}`);
        console.log(`stderr: ${stderr}`);
        parse.parse(lineToMessage, typeToCount, stdout);
        console.log(lineToMessage);
        console.log (typeToCount);
        callback(stdout);

    });
}

main(() => {
    exec('./cpp/main.out', (err, stdout, stderr) => {
        console.log(`stdout: ${stdout}`);
        console.log(`stderr: ${stderr}`);
    });
});




