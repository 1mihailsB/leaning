## Current 'business' idea for the proejct is video streaming platform with:
- ### Regular web <b>FE/BE+DB</b>, allowing users browse currently ongoing streams or replay recorded streams. Possibly view the <b>statistics</b> about time spent viewing specific streams or recordings. No need for other extra features than those, mentioned in this bullet point.
- ### <b>'Media server'</b> conntecting with streamer, receiving video/audio/other data for ongoing stream, possibly changing encoding/compression/quality. Possibly connecting to viewers and sending the currently ongoing stream to viewers, or sending stream data to some service that will distribute live stream to viewers (after the live stream data is compressed/converted by media server) <b>(Check wowza/red5pro/kurento services and RTMP, ffmpeg libs)</b>
- ### <b>'Recording server'</b> or mechanism to save the audio, video, chat and possibly meta data of ongoing stream so it can be replayed by viewers after stream is over. Could be part of <b>Media server</b> ?
- ### Custom made <b>event bus</b> (minimalistic Kafka) just for the minimal set of features of this project. To start communications before our services or initiate connections between users and some services other than Backend server behind the user facing Frontend. <b>Maybe some self made binary protocol ?</b>
- ### Some way for a person to start streaming - connect to media server and actually send video/audio/mic/camera/windows from persons machine. Maybe use existing solutions for that <b>(OBS)</b>. Maybe possible through the <b>browser instead of OBS, like Google meet and it's screen share</b>.

</br>
</br>

<div style="background-color: rgb(177, 242, 194, 0.2); height: 5em;" ></div>

</br>
</br>

# C++ practice
## Try making something lower level than what I can usually do in web dev work
## <b>For example:</b>
- ### Self made messag ebus
    - Start with minimal set of functionality necessary only for the whole project (to avoid spending many months just on even bus itself)
    - Message presistence - save incoming events/messages in permanent storage. For user statistics for example
    - Check existing protocol specifications, like <b>AMQP</b>. Check implementations in open source projects. Maybe worth to try making own protocol ?
- ### Think of uses cases in this project for a <b>cross-language library</b> made in c/c++ to be used in other languages, for example with <b>Java JNI</b>.

</br>
</br>

<div style="background-color: rgb(177, 242, 194, 0.2); height: 5em;" ></div>


</br >
</br>

# Web dev code artchitecture practice
## Make at least one of components (Backend for user facing Frontend/Media server/Recording server/other...) in the project without big frameworks. Practice code architecture
## <b>For example:</b>
### - <b>Keep in mind to keep feature set minimal, otherwise will take months/year+</b>
### - Make Backend service for the user website in Java without using Spring.
### - Consider making own <b>'Http server' or some 'http handler'</b> that can be used instead of Tomcat/Netty/Jetty. Even if it will have minimal features. Probably put it behind <b>Ngix</b>.
### - Should custom http handler be copmatible with <b>Java Servlets</b> ?
### - Make the service without IoC container. Get experience with organizing code for manual instantiation and management of objects and dependencies. Find out if frameworks really make work easier and the code base easier to reason about or the opposite.
### - <b>At the moment not very interested in frontend. Doesn't have to look good.</b>
### - Try <b>testing</b>, even just a few tests just to get some experience with testing. If it won't take much time - try to set up testing without testing frameworks/libraries, but it's not that important. Use library if custom would take much time.

</br>
</br>

<div style="background-color: rgb(177, 242, 194, 0.2); height: 5em;" ></div>

</br>
</br>

# Solution/infrastructure architecture practice (or whatever is the correct name for this)
## Get some experience in this project with designing the whole solution and putting all the services together in production environment
## <b>For example:</b>
### - CI/CD
### - Possibly configure (simple) <b>k8s</b> setup in cloud for all the services
### - Try to set up <b>application logic logging</b> and <b>environment performance logging</b>. Latter might be available OOTB in k8s.
### - Possibly create a setup for easy and fast local installation of the whole project locally (Docker or something else)

</br>
</br>

<div style="background-color: rgb(177, 242, 194, 0.2); height: 5em;" ></div>

</br>
</br>

# <span style="color: green; font-size: 5em;">Profit</span>
