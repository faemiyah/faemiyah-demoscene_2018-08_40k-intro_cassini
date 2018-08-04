#ifndef DIRECTION_HPP
#define DIRECTION_HPP

const int DIRECTION_SPLIT_SCENE_START = 25000 + (13 * 1400);
const int DIRECTION_SPLIT_SCENE_SPLIT_START = 4000;
const int DIRECTION_SPLIT_START = DIRECTION_SPLIT_SCENE_START + DIRECTION_SPLIT_SCENE_SPLIT_START;
const int DIRECTION_SPLIT_DURATION = 4000;
const int DIRECTION_SPLIT_END = DIRECTION_SPLIT_START + DIRECTION_SPLIT_DURATION;

/// Specifies the type of scene being displayed.
enum SceneEnum
{
  /// No scene, should not happen.
  NONE,
  /// Christiaan Huygens saturn sketch I.
  HUYGENS_SKETCH_01,
  /// Christiaan Huygens saturn sketch II.
  HUYGENS_SKETCH_02,
  /// Christiaan Huygens saturn sketch III.
  HUYGENS_SKETCH_03,
  /// Christiaan Huygens saturn sketch IV.
  HUYGENS_SKETCH_04,
  /// Christiaan Huygens saturn sketch V.
  HUYGENS_SKETCH_05,
  /// Christiaan Huygens saturn sketch VI.
  HUYGENS_SKETCH_06,
  /// Christiaan Huygens saturn sketch VII.
  HUYGENS_SKETCH_07,
  /// Christiaan Huygens saturn sketch VIII.
  HUYGENS_SKETCH_08,
  /// Christiaan Huygens saturn sketch IX.
  HUYGENS_SKETCH_09,
  /// Christiaan Huygens saturn sketch X.
  HUYGENS_SKETCH_10,
  /// Christiaan Huygens saturn sketch XI.
  HUYGENS_SKETCH_11,
  /// Christiaan Huygens saturn sketch XII.
  HUYGENS_SKETCH_12,
  /// Christiaan Huygens saturn sketch XIII.
  HUYGENS_SKETCH_13,
  /// Actually cassini's sketch.
  HUYGENS_SKETCH_14,
  /// Sketch scene, no text.
  HUYGENS_SKETCH_15,
  /// Space around Saturn.
  SPACE,
  /// Space around Saturn (simple version).
  SIMPLE,
  /// Enceladus surface.
  ENCELADUS,
  /// Saturn clouds.
  CLOUDS,
};

/// Direction data.
///
/// Direction is arranged as follows:
/// 0: Scene enumeration.
/// 1: Offset into data arrays at timestamp 0.
/// 2: Scene length (milliseconds).
/// 4-component camera position data for the scene until terminator.
/// 4-component camera eye point data for the scene until terminator.
/// 4-component camera up direction data for the scene until terminator.
int g_direction[] = 
{
  // Huygens sketch I.
  static_cast<int>(HUYGENS_SKETCH_01), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch II.
  static_cast<int>(HUYGENS_SKETCH_02), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch III.
  static_cast<int>(HUYGENS_SKETCH_03), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch IV.
  static_cast<int>(HUYGENS_SKETCH_04), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch V.
  static_cast<int>(HUYGENS_SKETCH_05), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch VI.
  static_cast<int>(HUYGENS_SKETCH_06), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch VII.
  static_cast<int>(HUYGENS_SKETCH_07), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch VIII.
  static_cast<int>(HUYGENS_SKETCH_08), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch IX.
  static_cast<int>(HUYGENS_SKETCH_09), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch X.
  static_cast<int>(HUYGENS_SKETCH_10), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch XI.
  static_cast<int>(HUYGENS_SKETCH_11), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch XII.
  static_cast<int>(HUYGENS_SKETCH_12), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch XIII.
  static_cast<int>(HUYGENS_SKETCH_13), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Space view very far from Saturn.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 3000,
  -58861, 24096, 145296, 3000,
  -60123, 24046, 144691, 3000,
  0, 0, 0, 0,
  -58457, 23902, 144402, 3000,
  -59714, 23861, 143798, 3000,
  0, 0, 0, 0,
  51, 981, -186, 3000,
  48, 983, -178, 3000,
  0, 0, 0, 0,

  // Space view less far from Saturn.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 3000,
  //-28642, 16987, 45878, 3000,
  //-28980, 17650, 45436, 3000,
  -21481, 12740, 34409, 3000,
  -21735, 13238, 34077, 3000,
  0, 0, 0, 0,
  //-28159, 16689, 45054, 3000,
  //-28494, 17329, 44623, 3000,
  -21119, 12517, 33790, 3000,
  -21370, 12997, 33467, 3000,
  0, 0, 0, 0,
  143, 955, -257, 3000,
  155, 948, -278, 3000,
  0, 0, 0, 0,

  // Vitu majesteettinen blarg.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 3000,
  6819, -7896, -14168, 3000,
  7793, -8157, -13484, 3000,
  0, 0, 0, 0,
  6571, -7318, -13390, 3000,
  7512, -7555, -12736, 3000,
  0, 0, 0, 0,
  -88, 656, -749, 3000,
  -76, 626, -775, 3000,
  0, 0, 0, 0,

  // Looking at probe (small) in front of Saturn.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 4000,
  -8748, 1214, 12000, 4000,
  -8963, 1399, 11724, 4000,
  0, 0, 0, 0,
  -8144, 1060, 11218, 4000,
  -8333, 1229, 10966, 4000,
  0, 0, 0, 0,
  18, 985, -172, 4000,
  30, 982, -186, 4000,
  0, 0, 0, 0,

  // Looking at probe (large) in front rings.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 4000,
  -4259, 1304, 7692, 4000,
  -4238, 1307, 7685, 4000,
  0, 0, 0, 0,
  -3858, 556, 8222, 4000,
  -3908, 579, 8286, 4000,
  0, 0, 0, 0,
  877, 481, 7, 4000,
  899, 437, 28, 4000,
  0, 0, 0, 0,

  // In Saturn's Shadow
  static_cast<int>(SPACE), static_cast<int>(Spline::BEZIER), 0, 8000,
  -29000, 11500, -27000, 4000,
  -25000, 11500, -31000, 4000, //SHADOW
  -21000, 11500, -35000, 4000,
  0, 0, 0, 0,
  0, 0, 0, 4000,
  0, 0, 0, 4000,
  0, 0, 0, 4000,
  0, 0, 0, 0,
  0, 1, 0, 4000,
  0, 1, 0, 4000,
  0, 1, 0, 4000,
  0, 0, 0, 0,

  // Titan.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 8000,
  -77279, 410, 95400, 8000,
  -77418, 354, 95377, 8000,
  0, 0, 0, 0,
  -76797, 31, 94610, 8000,
  -76797, 80, 94643, 8000,
  0, 0, 0, 0,
  -349, 744, -568, 8000,
  -375, 719, -583, 8000,
  0, 0, 0, 0,
#if 0
  // Titan.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 8000,
  -77578, 386, 95462, 8000,
  -77621, 357, 95406, 8000,
  0, 0, 0, 0,
  -76761, 182, 94923, 8000,
  -76754, 177, 94941, 8000,
  0, 0, 0, 0,
  -280, 676, -680, 8000,
  -194, 737, -646, 8000,
  0, 0, 0, 0,
#endif

  // Tethys.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 8000,
  29189, 45, -3666, 8000,
  29159, 43, -3668, 8000,
  0, 0, 0, 0,
  29159, -278, -4612, 8000,
  29263, -291, -4604, 8000,
  0, 0, 0, 0,
  -44, 945, -322, 8000,
  -45, 939, -341, 8000,
  0, 0, 0, 0,

  // Enceladus.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 4000,
  4778, 26, 23308, 4000,
  4772, 28, 23300, 4000,
  0, 0, 0, 0,
  3927, -426, 23571, 4000,
  3999, -483, 23676, 4000,
  0, 0, 0, 0,
  -428, 314, -846, 4000,
  -547, 241, -800, 4000,
  0, 0, 0, 0,

  // Enceladus sirppi.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 2000,
  4702, 53, 23282, 2000,
  4696, 45, 23281, 2000,
  0, 0, 0, 0,
  5219, -452, 23972, 2000,
  5286, -363, 23977, 2000,
  0, 0, 0, 0,
  660, -275, -697, 2000,
  652, -266, -709, 2000,
  0, 0, 0, 0,

#if 0
  // Enceladus surface geysers (pre-burst).
  static_cast<int>(ENCELADUS), static_cast<int>(Spline::LINEAR), 0, 6000,
  1103, 3526, -21042, 6000,
  312, 3321, -19932, 6000,
  0, 0, 0, 0,
  1664, 3111, -20326, 6000,
  873, 2906, -19216, 6000,
  0, 0, 0, 0,
  268, 911, 314, 6000,
  268, 911, 314, 6000,
  0, 0, 0, 0,
#else
  // Enceladus surface geysers (pre-burst).
  static_cast<int>(ENCELADUS), static_cast<int>(Spline::LINEAR), 0, 6000,
  13059, 2075, -34597, 6000,
  10861, 1929, -33506, 6000,
  0, 0, 0, 0,
  12710, 1604, -33787, 6000,
  10529, 1483, -32675, 6000,
  0, 0, 0, 0,
  129, 832, 539, 6000,
  142, 848, 512, 6000,
  0, 0, 0, 0,
#endif

  // Enceladus surface geysers (pre-burst).
  static_cast<int>(ENCELADUS), static_cast<int>(Spline::LINEAR), 0, 7000,
  -4014, 1496, -734, 7000,
  -4383, 1382, -186, 7000,
  0, 0, 0, 0,
  -3081, 1110, -632, 7000,
  -3451, 996, -84, 7000,
  0, 0, 0, 0,
  302, 905, 300, 7000,
  302, 905, 300, 7000,
  0, 0, 0, 0,

  // Enceladus surface geysers (burst begin).
  static_cast<int>(ENCELADUS), static_cast<int>(Spline::LINEAR), 0, 8000,
  3528, 936, -4630, 8000,
  2966, 924, -4004, 8000,
  0, 0, 0, 0,
  3927, 1017, -3735, 8000,
  3365, 1005, -3109, 8000,
  0, 0, 0, 0,
  -72, 996, 59, 8000,
  -80, 995, 54, 8000,
  0, 0, 0, 0,

  // Enceladus surface geysers (burst watch).
  static_cast<int>(ENCELADUS), static_cast<int>(Spline::LINEAR), 0, 7000,
  -2877, 2026, -1589, 7000,
  -3014, 1622, -1649, 7000,
  0, 0, 0, 0,
  -2190, 1671, -955, 7000,
  -2429, 2026, -946, 7000,
  0, 0, 0, 0,
  339, 929, 147, 7000,
  -199, 910, -361, 7000,
  0, 0, 0, 0,

  // Enceladus geysers space.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 6000,
  4698, 21, 23299, 6000,
  4697, 26, 23291, 6000,
  0, 0, 0, 0,
  5307, -136, 24076, 6000,
  5331, -194, 24032, 6000,
  0, 0, 0, 0,
  608, -534, -585, 6000,
  549, -544, -633, 6000,
  0, 0, 0, 0,

  // Pre-simple space.
  static_cast<int>(SPACE), static_cast<int>(Spline::LINEAR), 0, 2000,
  19901, 73505, 94435, 2000,
  21102, 74440, 94886, 2000,
  0, 0, 0, 0,
  19445, 72798, 93893, 2000,
  20649, 73733, 94342, 2000,
  0, 0, 0, 0,
  -303, 699, -646, 2000,
  -309, 700, -642, 2000,
  0, 0, 0, 0,

  // Simple space segment 1.
  static_cast<int>(SIMPLE), static_cast<int>(Spline::LINEAR), 0, 5000,
  18311, 72268, 93837, 5000,
  40389, 89448, 102137, 5000,
  0, 0, 0, 0,
  17852, 71561, 93300, 5000,
  39985, 88750, 101547, 5000,
  0, 0, 0, 0,
  -295, 696, -653, 5000,
  -411, 718, -559, 5000,
  0, 0, 0, 0,

  // Simple space segment 2.
  static_cast<int>(SIMPLE), static_cast<int>(Spline::LINEAR), 0, 5000,
  3268, 23976, -80003, 5000,
  -10023, 26774, -79426, 5000,
  0, 0, 0, 0,
  3320, 23662, -79055, 5000,
  -9819, 26403, -78519, 5000,
  0, 0, 0, 0,
  -328, 891, 312, 5000,
  -303, 856, -417, 5000,
  0, 0, 0, 0,

  // Simple space segment 3.
  static_cast<int>(SIMPLE), static_cast<int>(Spline::LINEAR), 0, 7000,
  17254, 16851, 33147, 7000,
  //18258, 14211, 33387, 7000,
  8813, 16812, 40617, 7000,
  0, 0, 0, 0,
  16826, 16476, 32325, 7000,
  //17830, 13836, 32565, 7000,
  8581, 16407, 39733, 7000,
  0, 0, 0, 0,
  -385, 913, -133, 7000,
  -298, 936, -186, 7000,
  0, 0, 0, 0,

  // Clouds 1.
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 6000,
  4277, 6100, 5397, 6000,
  4272, 5833, 2918, 6000,
  0, 0, 0, 0,
  4927, 5509, 5874, 6000,
  5034, 5311, 3300, 6000,
  0, 0, 0, 0,
  959, 269, 91, 6000,
  969, 226, 98, 6000,
  0, 0, 0, 0,

  // Clouds 2.
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 6000,
  -15356, 2360, -20901, 6000,
  -15783, 4430, -27896, 6000,
  0, 0, 0, 0,
  -14556, 1998, -20478, 6000,
  -15041, 3945, -27434, 6000,
  0, 0, 0, 0,
  526, 851, -3, 6000,
  612, 788, 64, 6000,
  0, 0, 0, 0,

  // Clouds 3.
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 2000,
  -22866, 6607, -40907, 2000,
  -21479, 5137, -40133, 2000,
  0, 0, 0, 0,
  -22003, 6105, -40841, 2000,
  -20546, 4963, -39855, 2000,
  0, 0, 0, 0,
  213, 476, 853, 2000,
  -102, 637, 764, 2000,
  0, 0, 0, 0,

  // Clouds (follow Cassini back).
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 7000,
  29655, 81741, -165885, 7000,
   6513, 60018, -150942, 7000,
  0, 0, 0, 0,
  //29110, 80923, -165703, 7000,
  28709, 81326, -165626, 7000,
   5848, 59480, -151348, 7000,
  0, 0, 0, 0,
  //43, 869, 492, 7000,
  270, 785, -556, 7000,
  35, 854, 518, 7000,
  0, 0, 0, 0,

  // Clouds (follow Cassini side).
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 6000,
   929, 58084, -154628, 6000,
  1374, 53406, -138193, 6000,
  0, 0, 0, 0,
  1716, 57829, -153957, 6000,
    2255, 53851, -138050, 6000,
  0, 0, 0, 0,
  -364, 644, 672, 6000,
  -371, 834, 407, 6000,
  0, 0, 0, 0,

  // Clouds (cassini smoke trail and explosion).
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 9000,
  -44005, 6907, -81352, 9000,
  -44253, 5997, -80718, 9000,
  0, 0, 0, 0,
  -43156, 7072, -80849, 9000,
  -43473, 6094, -80100, 9000,
  0, 0, 0, 0,
  -356, 930, 85, 9000,
  -336, 935, 106, 9000,
  0, 0, 0, 0,

  // Clouds (cassini fadeout).
  static_cast<int>(CLOUDS), static_cast<int>(Spline::LINEAR), 0, 7000,
  27574, 34599, -6791, 7000,
  28029, 33090, 1093, 7000,
  0, 0, 0, 0,
  26701, 34652, -6306, 7000,
  27053, 33191, 1293, 7000,
  0, 0, 0, 0,
  330, 828, 452, 7000,
  217, 833, 509, 7000,
  0, 0, 0, 0,

  // Huygens sketch XIV.
  static_cast<int>(HUYGENS_SKETCH_14), static_cast<int>(Spline::LINEAR), 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 1400,
  0, 0, 0, 0,
  0, 0, -1, 1400,
  0, 0, -1, 1400,
  0, 0, 0, 0,
  0, 1, 0, 1400,
  0, 1, 0, 1400,
  0, 0, 0, 0,

  // Huygens sketch XV.
  static_cast<int>(HUYGENS_SKETCH_15), static_cast<int>(Spline::LINEAR), 0, 10000,
  0, 0, 0, 10000,
  0, 0, 0, 10000,
  0, 0, 0, 0,
  0, 0, -1, 10000,
  0, 0, -1, 10000,
  0, 0, 0, 0,
  0, 1, 0, 10000,
  0, 1, 0, 10000,
  0, 0, 0, 0,

  // End of segemnts.
  0, 0, 0, 0,
};

/// Direction data resolved from scene.
class DirectionFrame
{
  private:
    /// Scene enumeration.
    SceneEnum m_scene;

    /// Number of milliseconds spent in this scene.
    int m_scene_time;

    /// Camera position.
    vec3 m_pos;

    /// Camera forward direction.
    vec3 m_fw;

    /// Camera up direction.
    vec3 m_up;

  public:
    /// Constructor.
    ///
    /// \param scene Scene being displayed.
    /// \param scene_time Time spent in this scene.
    /// \param pos Current position.
    /// \param eye Current eye point.
    /// \param up Current up direction.
    DirectionFrame(SceneEnum scene, int scene_time, const vec3& pos, const vec3& eye, const vec3& up) :
      m_scene(scene),
      m_scene_time(scene_time),
      m_pos(pos),
      m_fw(eye - pos),
      m_up(up)
    {
    }

  public:
    /// Accessor.
    ///
    /// \return Scene.
    SceneEnum getScene() const
    {
      return m_scene;
    }

    /// Accessor.
    ///
    /// \return Scene time.
    int getSceneTime() const
    {
      return m_scene_time;
    }

    /// Accessor.
    ///
    /// \return Position.
    const vec3& getPosition() const
    {
      return m_pos;
    }

    /// Accessor.
    ///
    /// \return Forward direction.
    const vec3& getForward() const
    {
      return m_fw;
    }

    /// Accessor.
    ///
    /// \return Up direction.
    const vec3& getUp() const
    {
      return m_up;
    }
};

/// Scene class.
///
/// Contains camera path and id.
class Scene
{
  private:
    /// Id of scene.
    SceneEnum m_id;

    /// Offset into the splines at timestamp 0.
    int m_offset;

    /// Scene length.
    int m_length;

    /// Camera position spline.
    Spline m_camera;

    /// Eye point spline.
    Spline m_eye;

    /// Up direction spline.
    Spline m_up;

  public:
    /// Constructor.
    ///
    /// \param data Input data.
    Scene(const int* data) :
      m_id(static_cast<SceneEnum>(data[0])),
      m_offset(static_cast<int>(data[2])),
      m_length(static_cast<int>(data[3])),
      m_camera(static_cast<Spline::SplineMode>(data[1])),
      m_eye(static_cast<Spline::SplineMode>(data[1])),
      m_up(static_cast<Spline::SplineMode>(data[1]))
    {
#if defined(USE_LD)
      if(0 > data[1])
      {
        std::ostringstream sstr;
        sstr << "invalid scene settings: " << data[0] << ", " << data[1] << ", " << data[2];
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      const int* eye_data = m_camera.readData(data + 4);
      const int* up_data = m_eye.readData(eye_data);
      m_up.readData(up_data);
    }

  public:
    /// Accessor.
    ///
    /// \return Scene ID.
    SceneEnum getId() const
    {
      return m_id;
    }

    /// Accessor.
    ///
    /// \return Scene length.
    int getLength() const
    {
      return m_length;
    }

    /// Resolve a camera position.
    ///
    /// \param stamp Position in frames.
    vec3 resolveCamera(int stamp) const
    {
      return m_camera.resolve(stamp + m_offset);
    }

    /// Resolve an eye point position.
    ///
    /// \param stamp Position in frames.
    vec3 resolveEye(int stamp) const
    {
      return m_eye.resolve(stamp + m_offset);
    }

    /// Resolve up direction.
    ///
    /// \param stamp Position in frames.
    vec3 resolveUp(int stamp) const
    {
      return m_up.resolve(stamp + m_offset);
    }
};

/// Intro direction.
///
/// Gives camera position and tells which scene it is.
class Direction
{
  private:
    /// Sequence of scenes.
    seq<Scene*> m_scenes;

  public:
    /// Constructor.
    Direction(const int* data)
    {
      const int* iter = data;

      for(;;)
      {
        m_scenes.push_back(new Scene(iter));

        // Skip 3 segments of spline data.
        const int* pos_data = iter + 4;
        const int* eye_data = next_segment(pos_data);
        const int* up_data = next_segment(eye_data);
        iter = next_segment(up_data);

        if(Spline::is_segment_end(iter))
        {
          break;
        }
      }
    }

    /// Destructor.
    ~Direction()
    {
      for(Scene *vv : m_scenes)
      {
        delete vv;
      }
    }
   
  public:
    /// Resolve the current scene frame.
    ///
    /// \param stamp Timestamp (frames).
    /// \return Scene frame containing all information.
    DirectionFrame resolveDirectionFrame(int stamp) const
    {
      for(unsigned ii = 0; (m_scenes.size() > ii); ++ii)
      {
        Scene* vv = m_scenes[ii];

        if(vv->getLength() > stamp)
        {
          SceneEnum id = vv->getId();
          vec3 pos = vv->resolveCamera(stamp);
          vec3 eye = vv->resolveEye(stamp);
          vec3 up = vv->resolveUp(stamp);

          // Calculate downwards to get total time spent in this type of scene.
          int total_time = stamp;
          for(;;)
          {
            if(0 >= ii)
            {
              break;
            }
            --ii;

            if(m_scenes[ii]->getId() == id)
            {
              total_time += m_scenes[ii]->getLength();
            }
          }

          return DirectionFrame(vv->getId(), total_time, pos, eye, up);
        }

        stamp -= vv->getLength();
      }

#if defined(USE_LD)
      BOOST_THROW_EXCEPTION(std::runtime_error("timestamp outside boundary"));
#else
      return DirectionFrame(NONE, 0, vec3(0.0f), vec3(0.0f), vec3(0.0f));
#endif
    }

  private:
    /// Get pointer past next termination point.
    ///
    /// \param data Data pointer.
    /// \return Pointer past termination point.
    const int* next_segment(const int *data)
    {
      const int *iter = data;
      for(;;)
      {
        const int *next = iter + 4;
        if(Spline::is_segment_end(iter))
        {
          return next;
        }
        iter = next;
      }
    }
};

#endif
