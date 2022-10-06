
const testSingleTimeout = true
const testMultipleTimeouts = false

if (testSingleTimeout) {
    setTimeout(() => {
        console.log("Single one second timeout function called")
    }, 1000)
}

if (testMultipleTimeouts) {
    const timeouts = [
        1000, // you
        500, // how
        200, // there
        100, // Hi
        800, // are
        2000 // : If you're seeing this, I'm not doing well
    ];

    const words = [
        "you",
        "how",
        "there",
        "Hi",
        "are",
        ": If you're seeing this, I'm not doing well (the timer didn't get removed)"
    ];

    const ids = [
        0,
        0,
        0,
        0,
        0,
        0,
    ]

    const done = [
        false,
        false,
        false,
        false,
        false,
        false
    ];

    const fullTimeout = timeouts.reduce((acc, curr) => (acc + curr));

    let message = " ";

    timeouts.forEach((duration, index) => {
        ids[index] = setTimeout(() => {
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

    setTimeout(() => {
        clearTimeout(ids[5])
    }, timeouts[5] - 500)

    function waitUntilAllDone() {
        setTimeout(() => {
            for (let i = 0; i < done.length - 1; i++) {
                if (!done[i]) {
                    waitUntilAllDone();
                    return;
                }
            }
            console.log(`All done`);

            // Wait for 2 more seconds to make sure the last word/timer doesn't fire.
            setTimeout(() => { }, 2000)
        }, 1000);
    }

    waitUntilAllDone();
}