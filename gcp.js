const vision = require('@google-cloud/vision').v1p3beta1;
const fs = require('fs');

// Creates a client
const client = new vision.ImageAnnotatorClient();

/**
 * TODO(developer): Uncomment the following line before running the sample.
 */
const fileName = `./testimg.jpg`;

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
        console.log(`Full text: ${fullTextAnnotation.text}`);
    })
    .catch(err => {
        console.error('ERROR:', err);
    });
