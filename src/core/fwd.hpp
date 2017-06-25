#pragma once

// core/obuject.hpp
class ObjectPropString;
class ObjectProp;
class Object;

// core/math.hpp
class Size2D;
class Region2D;
class FloatInVec;
class BoolInVec;
class Vec2;
class Vec3;
class Vec4;
class Matrix3x3;
class Matrix4x4;
class Vec3Pack8;

// core/rng.hpp
class XorShift128;

// core/ray.hpp
class Ray;
class Intersect;

// core/image.hpp
class Image;
class PixelLDR;
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




















