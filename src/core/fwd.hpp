#pragma once

// core/object.hpp
class ObjectPropString;
class ObjectProp;
class Object;
class SceneObject;

// core/math.hpp
struct Size2D;
struct Region2D;
struct FloatInVec;
struct BoolInVec;
struct Vec2;
struct Vec3;
struct Vec4;
struct Matrix3x3;
struct Matrix4x4;
struct Vec3Pack8;

// core/rng.hpp
class XorShift128;

// core/ray.hpp
struct Ray;
struct Intersect;

// core/image.hpp
class Image;
struct PixelLDR;
class ImageLDR;

// core/scene.hpp
class Scene;
class SceneGeometory;

// core/spectrum.hpp
class SpectrumRGB;
class SpectrumSampled;
class SpectrumSingleWavelength;
class SpectrumHerowavelength;

// core/taskscheduler.hpp
class SimpleTaskScheduler;

// core/transform.hpp
class Transform;

// core/util.hpp
class FileSystem;
class TimeUtil;

// bsdf/bsdf.hpp
class BSDF;
class BSDFs;
class MicrofacetDistribution;
class BlinnNDF;
class GeometricTerm;
class GeometricTermDefault;
class FresnelTerm;
class FresnelNone;
class FresnelConductor;
class FresnelDielectric;
class Lambertian;
class OrenNayar;
class Mirror;
class Glass;
class MicrofacetBSDF;
class TorranceSparrow;
class Ward;
class Walter;
class AshikhminShirley;
class DisneyBRDF;
class MeasuredBSDF;

// accelerator/bvh.hpp
class BVHBase;
class BruteForceBVH;
class SimpleBVH;
struct QBVHNode;
struct QBVHLeaf;
class QBVH;

// denoiser/denoiser.hpp
class Denoiser;
