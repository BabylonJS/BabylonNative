const touch = require("touch");
const glob = require("glob");

// npm packages by default have a time stamp of 10/26/1985 01:15 AM
// this will touch the babylonjs files such that incremental build works properly
glob("node_modules/babylonjs*/*.js", function (err, files) {
    if (err) {
        throw err;
    }

    for (const file of files) {
        console.log(`Touching ${file}`);
        touch.sync(file);
    }
});
