// Replace literal BigInt with Number as it's not supported with Chakra
// Literals (like 123n) are represented as BigIntLiteral nodes are transformed to NumericLiteral (123)
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
