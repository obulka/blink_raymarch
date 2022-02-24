// Source string for the Gain kernel.
static const char* const GainKernel = "\
kernel GainKernel : ImageComputationKernel<eComponentWise>\n\
{\n\
  Image<eReadWrite> dst;\n\
\n\
  param:\n\
    float multiply;\n\
\n\
  void define() {\n\
    defineParam(multiply, \"Gain\", 1.0f);\n\
  }\n\
\n\
  void process() {\n\
    dst() *= multiply;\n\
   }\n\
};\n\
";

// Source string for the Blur kernel.
static const char* const BlurKernel = "\
kernel BlurKernel : ImageComputationKernel<eComponentWise>\n\
{\n\
  Image<eRead, eAccessRanged1D, eEdgeClamped> src;  //the input image\n\
  Image<eWrite> dst;  //the output image\n\
\n\
  param:\n\
   int radius;\n\
   bool horizontal;\n\
\n\
  //In define(), parameters can be given labels and default values.\n\
  void define() {\n\
    defineParam(radius, \"Radius\", 10);\
    defineParam(horizontal, \"Horizontal\", true);\
  }\n\
\n\
  //The init() function is run before any calls to kernel().\n\
  void init() {\n\
    Axis axis = horizontal ? eX : eY;\n\
    src.setAxis(axis);\n\
    src.setRange(-radius, radius);\n\
  }\n\
\n\
  //The kernel function is run at every pixel to produce the output.\n\
  void process() {\n\
    ValueType(dst) value = 0;\n\
    for(int x = -radius; x <= radius; ++x) {\n\
      value += src(x);\n\
    }\n\
    dst() = value / (2 * radius + 1);\n \
  }\n\
};\n\
";
