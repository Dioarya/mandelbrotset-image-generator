cmake -S"$PSScriptRoot" -B"$PSScriptRoot/build" # Set source directory to be current working directory, build directory to be build folder inside working directory.
cmake --build "$PSScriptRoot/build" --target clean # clean
cmake --build "$PSScriptRoot/build" # build
