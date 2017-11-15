
### Gazebo + ROS

Follow the instructions for whichever ROS version you want to use.
```bash
for x in `ls models`; do ln -s `pwd`/models/$x ~/.gazebo/models/; done
sudo ln -s `pwd`/maze.launch /opt/ros/kinetic/share/gazebo_ros/launch/
sudo ln -s `pwd`/maze.world /usr/share/gazebo-7/worlds/
echo 'export LD_LIBRARY_PATH=/opt/ros/kinetic/lib:/usr/lib/x86_64-linux-gnu/gazebo-7/plugins' >> ~/.bashrc
source ~/.bashrc
roslaunch gazebo_ros maze.launch
```
Replace kinetic with the distribution of your choice

### simulate - a maze generator

Originally drive and lib items were part of a fun but out of scope simulator.  They are now used only to generate mazes for Gazebo

Link the FreeSans.ttf here so the current dumb ttf font open works
    ln -s /usr/share/fonts/truetype/freefont/FreeSans.ttf .

You will need SDL2::Raw, Cairo, and Algorithm::MinMaxHeap modules from perl6

    sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

run with `perl6 simulate`
