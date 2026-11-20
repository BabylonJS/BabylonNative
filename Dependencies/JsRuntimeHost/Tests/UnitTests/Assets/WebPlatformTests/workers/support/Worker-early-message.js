'use strict';

// WPT added this ordering regression after browsers raced Worker startup with
// an immediate postMessage. Gecko later fixed the complementary dead-worker
// path so pre-start/pending events are cleared when initialization never runs.
// https://github.com/web-platform-tests/wpt/commit/2060611f666a08629a55d5d594a0188c49c9ef5e
// https://github.com/mozilla/gecko-dev/commit/fd5b902f9f2ee1f9ed90e90a5843808422382987
// https://github.com/mozilla/gecko-dev/commit/b68bc791d026930001a3afbd2b3139ba58822435
const initializationEnds = Date.now() + 25;
while (Date.now() < initializationEnds) {}

onmessage = event => postMessage(event.data);
