# Chess Program in the Terminal

Don't take this too seriously. It's just a fun little project I'm doing to bump up my C knowledge.

## Building

```
git clone https://github.com/of-the-cross/termchess
cd termchess
chmod +x build.sh
./build.sh
./termchess
```

## Potential Errors

If you have a board rendered on your terminal but the pieces appear like nondescript boxes or something else, you probably don't have an emoji font installed. I have ttf-twemoji installed (twitter emojis) but you can install whatever you want.

If you get a jumbled mess of stuff where you have all the pieces rendered below the board, you're probably not on an advanced enough terminal. To run this program, you need a terminal that can do ANSI escape codes. That's what lets the program do all its crazy stuff. The terminal to an IDE is not an advanced terminal.
