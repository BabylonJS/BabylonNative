module.exports = function() {
  return {
    visitor: {
      BigIntLiteral(path) {
        const value = path.node.value;  // Get the BigInt literal value
        path.replaceWith({
          type: 'NumericLiteral',
          value: Number(value),  // Convert BigInt to Number
        });
      },
    },
  };
};
