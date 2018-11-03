const { exec } = require('child_process');

function main(callback) {
    exec('./cpp/compilecheck imgtext', (err, stdout, stderr) => {
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

        callback(stdout);

    });
}

main(() => {
    exec('./cpp/main.out', (err, stdout, stderr) => {
        console.log(`stdout: ${stdout}`);
        console.log(`stderr: ${stderr}`);
    });
});

