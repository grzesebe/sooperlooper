/*
*	File:		SooperLooperAU.h
*	
*	Version:	1.0
* 
*	Created:	7/2/05
*	
*	Copyright:  Copyright � 2005 Jesse Chappell, All Rights Reserved
* 
*/
#include "AUMIDIEffectBase.h"
#include "SooperLooperAUVersion.h"
#if AU_DEBUG_DISPATCHER
#include "AUDebugDispatcher.h"
#endif

#include "audio_driver.hpp"
#include <string>

#ifndef __SooperLooperAU_h__
#define __SooperLooperAU_h__

#pragma mark ____SooperLooperAU Parameters

namespace SooperLooper {
	class Engine;
	class MidiBridge;
};


// parameters

static CFStringRef kParameterOSCPortName = CFSTR("OSC Port");

enum {
	kParam_OSCPort = 0,
	//Add your parameters here...
	kNumberOfParameters
};

#define SL_MAXLOOPS 9

#pragma mark ____SooperLooperAU 
class SooperLooperAU 
	: public AUMIDIEffectBase,
public SooperLooper::AudioDriver
{
public:
	SooperLooperAU(AudioUnit component);

	virtual ~SooperLooperAU ();
	
	/*! @method Initialize */
	virtual ComponentResult		Initialize();
	
	/*! @method Cleanup */
	virtual void				Cleanup();
	
	
	/*! @method Reset */
	virtual ComponentResult		Reset(		AudioUnitScope 				inScope,
											AudioUnitElement 			inElement);
	
	
	virtual OSStatus			ProcessBufferLists(
												   AudioUnitRenderActionFlags &	ioActionFlags,
												   const AudioBufferList &			inBuffer,
												   AudioBufferList &				outBuffer,
												   UInt32							inFramesToProcess );
	
	
	//virtual AUKernelBase *		NewKernel() { return new SooperLooperAUKernel(this); }
	
	virtual ComponentResult		SaveState(				CFPropertyListRef *				outData);
	
	/*! @method RestoreState */
	virtual ComponentResult		RestoreState(			CFPropertyListRef				inData);
	
	
	virtual	ComponentResult		GetParameterValueStrings(AudioUnitScope			inScope,
														 AudioUnitParameterID		inParameterID,
														 CFArrayRef *			outStrings);
    
	virtual	ComponentResult		GetParameterInfo(AudioUnitScope			inScope,
												 AudioUnitParameterID	inParameterID,
												 AudioUnitParameterInfo	&outParameterInfo);
    
	virtual ComponentResult		GetPropertyInfo(AudioUnitPropertyID		inID,
												AudioUnitScope			inScope,
												AudioUnitElement		inElement,
												UInt32 &			outDataSize,
												Boolean	&			outWritable );
	
	virtual ComponentResult		GetProperty(AudioUnitPropertyID inID,
											AudioUnitScope		inScope,
											AudioUnitElement		inElement,
											void *			outData);
	/*! @method SetProperty */
	virtual ComponentResult		SetProperty(AudioUnitPropertyID 		inID,
											AudioUnitScope 				inScope,
											AudioUnitElement 			inElement,
											const void *				inData,
											UInt32 						inDataSize);
	
   	virtual	bool				SupportsTail () { return false; }
	
	/*! @method Version */
	virtual ComponentResult	Version() { return kSooperLooperAUVersion; }
	
	int		GetNumCustomUIComponents () { return 1; }
	
	void	GetUIComponentDescs (ComponentDescription* inDescArray) {
        inDescArray[0].componentType = kAudioUnitCarbonViewComponentType;
        inDescArray[0].componentSubType = SooperLooperAU_COMP_SUBTYPE;
        inDescArray[0].componentManufacturer = SooperLooperAU_COMP_MANF;
        inDescArray[0].componentFlags = 0;
        inDescArray[0].componentFlagsMask = 0;
	}
    
	/*
	virtual ComponentResult		RenderBus(				AudioUnitRenderActionFlags &	ioActionFlags,
														const AudioTimeStamp &			inTimeStamp,
														UInt32							inBusNumber,
														UInt32							inNumberFrames);
	*/
	/*! @method Render */
	virtual ComponentResult 	Render(AudioUnitRenderActionFlags &		ioActionFlags,
									   const AudioTimeStamp &			inTimeStamp,
									   UInt32							inNumberFrames);
	
protected:
		/*! @method HandleMidiEvent */
		virtual OSStatus	HandleMidiEvent(		UInt8 	inStatus,
													UInt8 	inChannel,
													UInt8 	inData1,
													UInt8 	inData2,
													long 	inStartFrame);
	
public:
	// SL AudioDriver stuff
	virtual bool initialize(std::string client_name="");
	virtual bool activate() { return true;}
	virtual bool deactivate() { return true;}
	
	virtual bool  create_input_port (std::string name, SooperLooper::port_id_t & port_id) ;
	virtual bool  create_output_port (std::string name, SooperLooper::port_id_t & port_id) ;
	
	virtual bool destroy_input_port (SooperLooper::port_id_t portid) ;
	virtual bool destroy_output_port (SooperLooper::port_id_t portid) ;
	
	virtual SooperLooper::sample_t * get_input_port_buffer (SooperLooper::port_id_t port, SooperLooper::nframes_t nframes) ;
	virtual SooperLooper::sample_t * get_output_port_buffer (SooperLooper::port_id_t port, SooperLooper::nframes_t nframes);
	virtual unsigned int get_input_port_count () { return _in_channel_id; }
	virtual unsigned int get_output_port_count () {return _out_channel_id; }
	
	virtual SooperLooper::nframes_t get_input_port_latency (SooperLooper::port_id_t portid);
	virtual SooperLooper::nframes_t get_output_port_latency (SooperLooper::port_id_t portid);

	virtual bool get_transport_info (SooperLooper::TransportInfo &info);
	
	virtual SooperLooper::nframes_t get_samplerate() { return (SooperLooper::nframes_t) GetSampleRate(); }
	virtual SooperLooper::nframes_t get_buffersize() { return (SooperLooper::nframes_t) GetMaxFramesPerSlice(); }

protected:
	
	// SL stuff
	SooperLooper::Engine *     _engine;
	SooperLooper::MidiBridge * _midi_bridge;
		
	pthread_t  _engine_thread;
	static void * _engine_mainloop (void * arg);

	volatile bool  _alive;
	std::string    _pending_restore;
	
	unsigned int _in_channel_id;
	unsigned int _out_channel_id;
	
	UInt32    _chancnt;
	AudioBufferList * _out_buflist[SL_MAXLOOPS];
	AudioBufferList * _in_buflist[SL_MAXLOOPS];
	AudioTimeStamp  _curr_stamp;
	UInt32       _last_framepos;
	UInt32       _last_rendered_frames;

	SInt16		 _stay_on_top;
	
	std::string  _guiapp_path;
protected:
		
		/*
		class SooperLooperAUKernel 
		: public AUKernelBase, public SooperLooper::AudioDriver		// most real work happens here
	{
public:
		SooperLooperAUKernel(AUEffectBase *inAudioUnit )
		: AUKernelBase(inAudioUnit)
	{

	}
		
		// *Required* overides for the process method for this effect
		
        virtual void 		Process(	const Float32 	*inSourceP,
										Float32		 	*inDestP,
										UInt32 			inFramesToProcess,
										UInt32			inNumChannels,
										bool			&ioSilence);
		
        virtual void		Reset();
		
		//private: //state variables...
	};
		 */
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#endif