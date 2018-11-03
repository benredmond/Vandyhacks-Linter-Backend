const vision = require('@google-cloud/vision').v1p3beta1;
const fs = require('fs');

// Creates a client
const client = new vision.ImageAnnotatorClient();
const fileName = `./IMG_1170.jpg`;

const request = {
    image: {
        content: fs.readFileSync(fileName),
    },
    feature: {
        languageHints: ['en-t-i0-handwrit'],
    },
};
client
    .documentTextDetection(request)
    .then(results => {
        const fullTextAnnotation = results[0].fullTextAnnotation;
        let content = '#include <iostream>\nint main() {\n' + fullTextAnnotation.text + 'return 0;\n}';
        fs.writeFile('./cpp/imgtext.cpp', content);
        console.log(`Full text: ${fullTextAnnotation.text}`);
    })
    .catch(err => {
        console.error('ERROR:', err);
    });
