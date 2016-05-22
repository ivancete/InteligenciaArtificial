#
touch bots/mybot/aiml/tmp
java -cp lib/Ab.jar Main bot=mybot action=chat trace=false morph=false
rm bots/mybot/aiml/tmp



