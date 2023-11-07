export TERM=xterm # disable color
export PAGER=""

if [ -e ~/.config/helix/exec.env ]; then
  source ~/.config/helix/exec.env 2> /dev/null
fi

if [ -e "$PWD" ]; then
  cd "$PWD"
else
  PWD=$(pwd)
fi

stdin=$(cat)
echo "$stdin"
echo
eval "$stdin"
echo '['"$PWD"']$'

# only save PWD for now
# saving PATH breaks the `source` cmd on WSL
env | grep "PWD" > ~/.config/helix/exec.env 2>/dev/null
