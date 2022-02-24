// Copyright (c) 2013 The Foundry Visionmongers Ltd.  All Rights Reserved.

//DDImage includes
#include "DDImage/PlanarIop.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"
#include "DDImage/Blink.h"

//Blink API includes
#include "Blink/Blink.h"

//Header containing the source strings for the Blink kernels we will use for image processing
#include "GPUNode.h"

//Some information about the plug-in 
static const char* const CLASS = "GPUNode";
static const char* const HELP = "Example of a simple box blur and gain operator that uses the Blink API.";

/*
 * Simple Blink API example plug-in that does a two-pass box blur and applies a gain.
 */

// Blink plug-ins do their image processing on tiles rather than scanlines. They should always inherit from
// DD::Image::PlanarIops to get access to ImagePlanes, which can then be wrapped in Blink::Images for
// Blink-based image processing.
class GPUNode : public DD::Image::PlanarIop
{
protected:
    // Reference to the GPU device to process on.
    Blink::ComputeDevice _gpuDevice;

    // Whether to process on the GPU, if available
    bool _useGPUIfAvailable;

    // The radius to use for the blur kernel.
    int _radius;

    // The amount of gain to apply.
    float _gain;

    // This holds the ProgramSource for the blur kernel.
    Blink::ProgramSource _blurProgram;
    // This holds the ProgramSource for the gain kernel.
    Blink::ProgramSource _gainProgram;

public:
    // Constructor. Initialize user controls and local variables to their default values.
    GPUNode(Node* node) 
        : PlanarIop(node)
        , _gpuDevice(Blink::ComputeDevice::CurrentGPUDevice())
        , _useGPUIfAvailable(true)
        , _blurProgram(BlurKernel)
        , _gainProgram(GainKernel)
    {
        _radius = 3;
        _gain = 2.0f;
    }

    // This function creates the controls that will be exposed to the user.
    void knobs(DD::Image::Knob_Callback f);

    void _validate(bool);

    void getRequests(const DD::Image::Box& box, const DD::Image::ChannelSet& channels, int count, DD::Image::RequestOutput &reqData) const;

    // Whether to process in stripes or full-frame.
    virtual bool useStripes() const;

    // Set the stripe height to use for processing.
    virtual size_t stripeHeight() const;

    // This function does all the work in a PlanarIop. It is guaranteed that outputPlane here will never have more than 
    // the 4 channels which can be stored in a Blink::Image.
    void renderStripe(DD::Image::ImagePlane& outputPlane);

    // Return the name of the class.
    const char* Class() const { return CLASS; }
    // Return a brief description of the node.
    const char* node_help() const { return HELP; }

    // Information to the plug-in manager of DDImage/Nuke.
    static const Iop::Description description;
};



void GPUNode::knobs(DD::Image::Knob_Callback f)
{
    // Add GPU knobs
    Newline(f, "Local GPU: ");
    const bool hasGPU = _gpuDevice.available();
    std::string gpuName = hasGPU ? _gpuDevice.name() : "Not available";
    Named_Text_knob(f, "gpuName", gpuName.c_str());
    Newline(f);
    Bool_knob(f, &_useGPUIfAvailable, "use_gpu", "Use GPU if available");
    Divider(f);

    // Add a parameter for the blur radius.
    Int_knob(f, &_radius, "radius");
    Tooltip(f, "The radius of the blur.");

    // Add a parameter for the gain amount.
    Float_knob(f, &_gain, DD::Image::IRange(0, 10), "gain");
    Tooltip(f, "The amount of gain to apply.");
}

void GPUNode::_validate(bool for_real)
{
    // Copy bbox channels etc from input0, which will validate it.
    copy_info(); 

    // Pad the output area this node can produce by the size of the blur radius.
    info_.pad(_radius);  
}

void  GPUNode::getRequests(const DD::Image::Box& box, const DD::Image::ChannelSet& channels, int count, DD::Image::RequestOutput &reqData) const
{
    DD::Image::Box inputBox = box;
    // Request _radius extra pixels around the input.
    inputBox.expand(_radius);
    reqData.request(&input0(), inputBox, channels, count);
}


/*
 * Override of PlanarIop function renderStripe(...). This does the image processing to produce a stripe of the output.
 */
void GPUNode::renderStripe(DD::Image::ImagePlane& outputPlane)
{
    // Work out the bounds of the area we need to fetch from the input. This will depend on the output bounds.
    DD::Image::Box inputBox = outputPlane.bounds();
    inputBox.expand(_radius);
    inputBox.intersect(input0().info());

    // Make an ImagePlane that satisfies this description.
    DD::Image::ImagePlane inputPlane(
        inputBox, // the bounds of the input we want to fetch
        outputPlane.packed(), // input data should be packed in the same way as the output plane
        outputPlane.channels(), // input data should have the same channels as the output plane
        outputPlane.nComps() // input data should have the same number of components as the output plane
    );

    // Fetch the data from input0 into our ImagePlane.
    input0().fetchPlane(inputPlane);

    // This function must be called on the output plane before writing to it.
    outputPlane.makeWritable();


    // Wrap the input and output planes as Blink images. The underlying data stays the same. 
    Blink::Image outputPlaneAsImage;
    Blink::Image inputPlaneAsImage;
    bool success = (DD::Image::Blink::ImagePlaneAsBlinkImage(outputPlane, outputPlaneAsImage) &&
                                    DD::Image::Blink::ImagePlaneAsBlinkImage(inputPlane, inputPlaneAsImage));
    
    // Check the fetch succeeded.
    if (!success) {
        error("Unable to fetch Blink image for image plane.");
        return;
    }

    // Has the user requested GPU processing, and is the GPU available for processing on?
    bool usingGPU = _useGPUIfAvailable && _gpuDevice.available();

    // Make a temporary image to blur into, on the GPU or CPU as appropriate. This should have the same layout as the input image.
    Blink::Image intermediateImage = usingGPU ? inputPlaneAsImage.makeLike(_gpuDevice) : inputPlaneAsImage.makeLike(Blink::ComputeDevice::CurrentCPUDevice());

    // Get a reference to the ComputeDevice to do our processing on. 
    Blink::ComputeDevice computeDevice = usingGPU ? _gpuDevice : Blink::ComputeDevice::CurrentCPUDevice();

    // Distribute our input image from the device used by NUKE to our ComputeDevice.
    Blink::Image inputImageOnComputeDevice = inputPlaneAsImage.distributeTo(computeDevice);

    // This will bind the compute device to the calling thread. This bind should always be performed before
    // beginning any image processing with Blink.
    Blink::ComputeDeviceBinder binder(computeDevice);

    // Make a vector containing the images we want to run the first kernel over.
    std::vector<Blink::Image> images;
    images.push_back(inputImageOnComputeDevice);
    images.push_back(intermediateImage);

    // Make a Blink::Kernel from the source in _blurProgram to do the horizontal blur.
    Blink::Kernel horizontalBlurKernel(_blurProgram, 
        computeDevice, 
        images, 
        kBlinkCodegenDefault);
    horizontalBlurKernel.setParamValue("Radius", _radius);
    horizontalBlurKernel.setParamValue("Horizontal", true);

    // Run the horizontal blur kernel. Because we haven't specified the bounds to iterate over, these
    // will be the bounds of the last image passed in - in this case, the intermediateImage.
    horizontalBlurKernel.iterate();

    // If we are on the GPU, we need to make an output image, otherwise we can just use the one from NUKE.
    Blink::Image outputImage = usingGPU ? outputPlaneAsImage.makeLike(_gpuDevice) : outputPlaneAsImage;

    // Reset the images vector to hold the images needed for the vertical blur.
    images.clear();
    images.push_back(intermediateImage);
    images.push_back(outputImage);

    // Make a Blink::Kernel from the source in _blurProgram to do the vertical blur.
    Blink::Kernel verticalBlurKernel(_blurProgram, 
        computeDevice, 
        images, 
        kBlinkCodegenDefault);
    verticalBlurKernel.setParamValue("Radius", _radius);
    verticalBlurKernel.setParamValue("Horizontal", false);

    // Run the vertical blur kernel. Because we haven't specified the bounds to iterate over, these
    // will be the bounds of the last image passed in - in this case, the outputImage.
    verticalBlurKernel.iterate();
    
    // The gain kernel requires only the output image as input/output.
    images.clear();
    images.push_back(outputImage);

    // Make a Blink::Kernel from the source in _gainProgram to do the gain.
    Blink::Kernel gainKernel(_gainProgram, 
        computeDevice, 
        images, 
        kBlinkCodegenDefault);
    gainKernel.setParamValue("Gain", _gain);
    
    // Run the gain kernel over the output image.
    gainKernel.iterate();

    // If we're using the GPU, copy the result back to NUKE's output plane.
    if (usingGPU) {
        outputPlaneAsImage.copyFrom(outputImage);
    }
}

/*
 * Override of PlanarIop function; returns true.
 * renderStripe() will be called on imageplanes of size info().w() x stripeHeight().
 */
bool GPUNode::useStripes() const
{
    return true;
}

/*
 * Override of PlanarIop function;
 * returns a fixed stripe height of 256.
 */
size_t GPUNode::stripeHeight() const
{
    return 256;
}

/* This is a function that creates an instance of the operator, and is
     needed for the Iop::Description to work.
 */
static DD::Image::Iop* GPUNodeCreate(Node* node)
{
    return new GPUNode(node);
}

/* The Iop::Description is how NUKE knows what the name of the operator is,
     how to create one, and the menu item to show the user. The menu item may be
     0 if you do not want the operator to be visible.
 */
const DD::Image::Iop::Description GPUNode::description(CLASS, "Filter/GPUNode",
                                                                                                                                GPUNodeCreate);
