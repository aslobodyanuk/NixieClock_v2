void glitchTick() {
  if (!glitchFlag && secs > 7 && secs < 55) {
    if (glitchTimer.isReady()) {
      initializeGlitches();
    }
  } else if (glitchFlag && glitchTimer.isReady()) {
    indiDimm[glitchIndic] = indiState * indiMaxBright;
    indiState = !indiState;
    setRandomGlitchInterval();
    glitchCounter++;
    if (glitchCounter > glitchMax) {
      glitchTimer.setInterval(random(GLITCH_MIN * 1000L, GLITCH_MAX * 1000L));
      glitchFlag = false;
      indiDimm[glitchIndic] = indiMaxBright;
    }
  }
}

void initializeGlitches() {
  glitchFlag = true;
  indiState = 0;
  glitchCounter = 0;
  glitchMax = random(3, 9);
  glitchIndic = random(0, 4);
  setRandomGlitchInterval();
}

void setRandomGlitchInterval() {
  glitchTimer.setInterval(random(30, 101));
}