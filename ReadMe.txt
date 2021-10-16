ttaplugins-winamp v3.62
=====================================================

*This plugin 
is based on TAU Software(http://en.true-audio.com/)
TTA plug-in ver 3.5 for Winamp 2,5.
The modification is ID3v2 Tag read/write function using 
taglib(https://github.com/taglib/taglib).
And add read metadata (include album art) for MediaLibray.
Additionally, tta encode plugin is included.
This plugin performs Winamp Ver.2.9 or newer and tested by Winamp Ver.5.8.

*Install
 Please copy in_tta.dll and enc_tta.dll to Plugins folder in Winamp.

*Release Notes
2021-08-18 3.62       Change upstream taglib master (based on 2021/07/10  4:47:36 Commit hash:f58161511050ac6c060d28e75dcd5c5a3af7279c)(in_tta.dll)
                      Change compiler to Visual Studio 2019 (Version 16.11.1)(in_tta.dll, enc_tta.dll)
2021-05-09 3.61       Change upstream taglib master (based on 2021/05/02 19:14:27 Commit hash:4971f8fb032f0cde8c2437cea44b6217b0f37269)(in_tta.dll)
2021-04-29 3.60       Change compiler to Visual Studio 2019 (Version 16.9.4)(in_tta.dll, enc_tta.dll)
2021-04-16 3.59       Change upstream taglib master (based on 2021/03/30 03:21:59 Commit hash:1644c0dd87f40a2492bde4011fe72257fc72ee0e)(in_tta.dll)
                      Change compiler to Visual Studio 2019 (Version 16.9.3)(in_tta.dll, enc_tta.dll)
2021-03-16 3.58       Change upstream taglib master (based on 2021/02/17 16:20:05 Commit hash:b5cd4c40e25c8026b42c4e71b4df63cbeb7cfe6a)(in_tta.dll)
                      Change compiler to Visual Studio 2019 (Version 16.9.1)(in_tta.dll, enc_tta.dll)
                      Update NSIS script for Unicode target
2021-03-01 3.57       Change upstream taglib master (based on 2021/02/16 21:00:49 Commit hash:340ec9932a7c782a2f876c0efea233a43502c88e)(in_tta.dll)
                      Change compiler to Visual Studio 2019 (Version 16.8.6)(in_tta.dll, enc_tta.dll)
2019-06-27 3.55       Change upstream taglib master (based on 2019/05/31 20:21:16 Commit hash:ba7adc2bc261ed634c2a964185bcffb9365ad2f4)(in_tta.dll)
                      Change compiler to Visual Studio 2019 (Version 16.1.4)(in_tta.dll, enc_tta.dll)
2019-02-01 3.54       Change upstream taglib master (based on 2018/10/28 22:43:45 Commit hash:5cb589a5b82c13ba8f0542e5e79629da7645cb3c)(in_tta.dll)
                      Change NSIS verion 3.04 and Update install script for downloading VC2017 redistributable under proxy.
                      Change compiler to Visual Studio 2017 (version 15.9.6)(in_tta.dll, enc_tta.dll)
2018-01-01 3.53       Change upstream taglib master (based on 2017/11/20 8:03:52 Commit hash:a80093167f93f4147ec153693d871780874f9747)(in_tta.dll)
2017-12-31 3.52       Change upstream taglib master (based on 2017/10/01 0:15:41 commit Hash:cb9f07d9dcd791b63e622da43f7b232adaec0a9a)(in_tta.dll)
                      Change compiler to Visual Studio 2017 (version 15.5.2)(in_tta.dll, enc_tta.dll)
                      Add reload tag when write it(in_tta.dll)
2017-08-30 3.51       Change upstream taglib stable (based on 2017/06/13 17:22:00 commit Hash:46483948413c1330c036240ed89429a1c6905843)(in_tta.dll)
                      Change compiler to Visual Studio 2017 (version 15.3)(in_tta.dll, enc_tta.dll)
2017-06-29 3.50       Change upstream taglib stable (based on 2017/06/09 17:52:56 commit Hash:c8bcd153fe4a1c9c792dd8cd404226b19a3fc1c7)(in_tta.dll)
                      Change compiler to VC2017(in_tta.dll, enc_tta.dll)
					  Change compile option to use SSE2(in_tta.dll)
2017-06-06 3.49       Change decode engine from ttaplugin-winamp-3.5 to libtta-cpp-2.3(in_tta.dll)
2017-04-19 3.48       Change playing decode engine from libtta-cpp-2.3 to ttaplugin-winamp-3.5 base version(in_tta.dll)
2017-04-07 3.47       Change compiler to VC2010 for execute in Windows7 (in_tta.dll,enc_tta.dll)
                      Use dynamic zlib library(in_tta.dll)
2017-02-25 3.46       Change upstream taglib stable (based on 2017/02/24 15:47:30 commit Hash:4891ee729d5bd98fbe8ff82b89e9758f00d18815)(in_tta.dll)
                      Use static libtta library(in_tta.dll)
2016-12-14 3.45       Fix memory leak(libtta.dll)
                      Fix memory leak(enc_tta.dll)
2016-12-12 3.44       Improve code stability(in_tta.dll)
                      Remove config dialog(in_tta.dll)
2016-12-10 3.43       Change upstream taglib stable (based on 2016/12/05 11:02:59 commit Hash:36ccad2bd4b5b8aec1e547faef3bfe0269316ae9)(in_tta.dll)
                      Reduce memory usage in transcoding(in_tta.dll)
                      Improve code stability(in_tta.dll)  
2016-06-07 3.42       Change upstream taglib stable (based on 2016/05/14 10:46:42 commit Hash:3e47a036fbbe5117377018fbbe4cbdd49319196e)(in_tta.dll)
2016-06-06 3.41       Change decode engine from ttaplugin-winamp-3.5 to libtta-cpp-2.3(in_tta.dll)
                      Fix incorrect bitrate in metadata(in_tta.dll)
					  Change temporary directory for encoding to output directory(enc_tta.dll)
2016-05-17 3.40       Fix minor bug.(enc_tta.dll)
2016-05-15 3.39       Fix minor bug. Add type of metadata for Read/Write(in_tta.dll)
2015-12-30 3.38		  Fix download URI of VC2015 runtime redistibutable.
2015-10-10 3.37       Fix a issue that is to reset playing time when playing and encoding tta files in the same time(in_tta.dll)
2015-09-04 3.36       Change rule of version number
                      Change compile option of library(tag.dll)
                      Change compile option of library(in_tta.dll)
2015-08-27 3.5 Beta31 Fix saving of tag data(in_tta)
                      Change showing overall bitrate to instantaneous bit rate in player(in_tta)
2015-08-27 3.5 Beta30 Fix transcoding(in_tta)
2015-08-24 3.5 Beta29 Change playing decode engine from libtta-cpp-2.3 to ttaplugin-winamp-3.5 base version(in_tta.dll)
2015-08-24 3.2 Beta28 Change libtta.dll to enable SSE4 version(libtta.dll)
2015-08-24 3.2 Beta27 Reduce number of accessing file at getting meta data(in_tta.dll)
2015-08-20 3.2 Beta26 Fix CPU overload because of accessing file at getting meta data(in_tta.dll)
2015-08-13 3.2 Beta25 Fix encoder was creating incomplete tta file(enc_tta.dll)
                      Fix function name for valid filename in trans coding(in_tta.dll)
		    		  Code cleanup(in_tta.dll)
2015-08-12 3.2 Beta24 Code cleanup(in_tta.dll)
                      Code cleanup(enc_tta.dll)
2015-08-11 3.2 Beta23 Change native Unicode compile(in_tta.dll)
                      Detach each plugin (in_tta and enc_tta)
2015-08-11 3.2 Beta22 Fix saves AlbumArt in in_tta.dll
2015-08-11 3.2 Beta21 Add enc_tta.dll
                      Fix some transcoding issue in in_tta.dll
				      Detach libtta from in_tta.dll. Use libtta.dll
                      Detach taglib from in_tta.dll. Use tag.dll
                      Change upstream taglib stable(2015/08/10 0:50:13 commit Hash:bc106ad81e1015c896b32ae1fec6cb3c3894ac84)
2015-08-04 3.2 Beta20 Fix some stabilize issues
2015-08-04 3.2 Beta19 Fix transcoding
2015-08-02 3.2 Beta18 Fix freeze when playing 24bit/sample file.
                      Remove title name function and uses Winamp itself
	     			  Reduce file access when reading metadata
		    		  Change plugin UNICODE base
                      Change upstream taglib stable(2015/06/19 16:12:32 commit Hash:e90b5e5f2faded598688c48dd3bdd2fc6cd5cbab)
		    		  Change uses zlib.dll in Winamp
		    		  Change compiler to VS2015
2015-05-27 3.2 Beta17 Change base library to libtta++2.3
                      Change base taglib stable(2015/05/25 21:41:37 commit Hash:c5f2e9342dfdf9a94dbcbf70fc3861b2221d1b39)
2014-11-28 3.2 Beta16 Change base library to libtta++2.2
                      Change base taglib stable(2014/10/23 2:31:14 commit Hash:072851869a2980dab4aefd471ad7dd707993d74f)
			    	  Change compiler to VS2013
2013-08-22 3.2 Beta15 Change base taglib stable(commit Hash:fddf3ed51b705d69e12df4a0cba01984ce82c638)
                      Fix memory leak in transcoding
				      Change DLL taglib to static taglib.
2011-12-10 3.2 Beta14 Revert multibyte version
                      Fix exception from libtta
2011-12-03 3.2 Beta13 Unicode Plugin version
                      Add exception processs from decoder
2011-11-27 3.2 Beta12 Fix freeze when invalid file will open
                      Fix hung up when reading file with no album art
2011-11-21 3.2 Beta11 Change base library to libtta++2.1
                      Change taglib included to use external taglib dll
                      Fix memory leak when reading album art
2011-11-17 3.2 Beta10 Change base taglib 1.7.0
                      Change compiler to VS2010
2010-11-29 3.2 Beta9  Add reading album art
2009-04-09 3.2 Beta8  Change compiler to VC2008
                      Change Year tag from TDRL to TDRC in ID3 Ver2.4
2007/10/23 3.2 Beta7  Fix incorrect Seek position
                      (Thanks to h0shu (http://hoshustep.hp.infoseek.co.jp/dust.html))
                      Add format conversion function
2006/07/03 3.2 Beta6  Change read year metadata
2006/02/23 3.2 Beta5  Fix incorrect Beta4
2005/12/19 3.2 Beta4  Fix problem that other program cannot access playing tta file
2005/12/16 3.2 Beta3  Fix reading ID3v2 Ver2.3 (incorrect decoding frame size)
2005/12/14 3.2 Beta2  Initial release

*Copying
This libray is distributed under LGPL2.1

*Acknowledgement

Refer in_mpg123.dll source code made by Otachan(http://www3.cypress.ne.jp/otachan/)in reading metadata for media library.

Refer STEP(SuperTagEditor-kai Plugin Version (Nightmare)) source code made by Haseta(http://haseta2003.hp.infoseek.co.jp/) in ID3v2 related.

Refer patch made by h0shu(http://hoshustep.hp.infoseek.co.jp/dust.html) in fixing seek problem.

Yamagata Fumihiro
mailto:bunbun042000@gmail.com
