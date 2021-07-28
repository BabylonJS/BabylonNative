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

const fullTimeout = timeouts.reduce((acc, curr) => (acc + curr));

let message = " ";

timeouts.forEach((duration, index) => {
    setTimeout(() => {
        message = `${message} ${words[index]}`;
    }, duration);
});

const startTime = new Date().getTime();
setTimeout(() => {
    console.log(message);
    const endTime = new Date().getTime();
    console.log(`Elapsed time: ${endTime - startTime} (Expected: ${fullTimeout})`);
}, fullTimeout);

function ping() {
    setTimeout(() => {
        console.log("Ping");
        ping();
    }, 1000);
}

ping();