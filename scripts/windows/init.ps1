echo "Initializing project..."
cd $PSScriptRoot
cd ../..
$projectPath = $pwd.Path

echo "Creating build system..."
mkdir -p build/debug
cd build/debug
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug $projectPath
echo "Finished creating build system."

cd $projectPath
echo "Finished project initialization."
