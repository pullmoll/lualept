# lualept

## Lua language bindings for Leptonica

This is a WIP (work in progress) to create a Lua (https://www.lua.org/) wrapper
for Leptonica (http://leptonica.org/).

The project's goal is to cover most if not all of Leptonica's huge number of
types and functions in a marriage of Lua's and Leptonica's concepts.

The reason I started this was because I can use the result in a project I'm doing
paid work for. This project already uses Leptonica and a C++ wrapper around some of its
functions. Instead of inventing my own scripting language for running scripts doing
things with Leptonica inside this project, I thought that using Lua for this purpose
is most probably a better choice.

We already use Lua in a small portion of another part of that project.

This is my first *larger* Lua and C-Functions project and I may well be doing some
things in an awkward way or even outright wrong, so feel free to point your fingers
at me and tell me where I'm missing the point.

Still, the project already runs some script (lua/script.lua) which I use for testing
the bindings as I write the wrappers.

Jürgen Buchmüller <pullmoll@t-online.de>
