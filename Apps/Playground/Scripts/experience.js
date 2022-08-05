const timeouts = [
    1000, // you
    500, // how
    200, // there
    100, // Hi
    800, // are
];

const words = [
    "you",
    "how",
    "there",
    "Hi",
    "are"
];

const done = [
    false,
    false,
    false,
    false,
    false
];

const fullTimeout = timeouts.reduce((acc, curr) => (acc + curr));

let message = " ";

timeouts.forEach((duration, index) => {
    setTimeout(() => {
        message = `${message} ${words[index]}`;
        done[index] = true
    }, duration);
});

const startTime = new Date().getTime();
setTimeout(() => {
    console.log(message);
    const endTime = new Date().getTime();
    console.log(`Elapsed time: ${endTime - startTime} (Expected: ${fullTimeout})`);
}, fullTimeout);

function waitUntilAllDone() {
    setTimeout(() => {
        for (let i = 0; i < done.length; i++) {
            if (!done[i]) {
                waitUntilAllDone();
                return;
            }
        }
        console.log(`All done`);
    }, 1000);
}

waitUntilAllDone();