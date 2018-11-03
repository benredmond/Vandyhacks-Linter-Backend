module.exports = {
    calculateScore : function (typeToCount, callback) {
        // typeToCount = JSON.parse(typeToCount);
        let warningNum = typeToCount.warning;
        let errorNum = typeToCount.error;
        let styleNum = typeToCount.style;

        let warningPercentage = 30
        let errorPercentage = 60
        let stylePercentage = 10

        let warningRatio = 1 - (1 / (warningPercentage / 10))
        let errorRatio = 1 - (1 / (errorPercentage / 10))
        let styleRatio = 1 - (1 / (stylePercentage / 10))

        let warningScore = (warningPercentage / 10) - ((1 - Math.pow(warningRatio, warningNum)) / (1 - warningRatio))
        let errorScore = (errorPercentage / 10) - ((1 - Math.pow(errorRatio, errorNum)) / (1 - errorRatio))
        let styleScore = (stylePercentage / 10) - ((1 - Math.pow(styleRatio, styleNum)) / (1 - styleRatio))

        callback(warningScore + errorScore + styleScore);

    }
}