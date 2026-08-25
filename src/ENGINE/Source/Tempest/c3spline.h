#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"
#include "Storm/h/stpl.h"

namespace NTempest {

class C3Spline {
public:
    enum EvalType {
        EVAL_PARAMETRIC = 0,
        EVAL_ARCLENGTH = 1,
        EVAL_COUNT = 2,
    };

    static constexpr unsigned int DEFAULT_STEPS = 20;

    virtual ~C3Spline() = default;
    virtual C3Vector Evaluate(float t) const = 0;
    virtual C3Vector EvaluateDer1(float t) const = 0;
    virtual C3Vector EvaluateDer2(float t) const = 0;
    void SetPoint(unsigned int index, const C3Vector& point);
    void SetPoints(const C3Vector* source, unsigned int count);
    C3Vector Pos(float t, EvalType type = EVAL_PARAMETRIC) const;
    C3Vector Vel(float t, EvalType type = EVAL_PARAMETRIC) const;
    void Frame(float t, C3Vector& tangent, C3Vector& normal, C3Vector& binormal) const;
    float Curvature(float t) const;
    float ILength() const;
    float SegLength(unsigned int segment, unsigned int steps = DEFAULT_STEPS) const;
    float ParametricSegT(float t, unsigned int& segment) const;
    float ArclengthSegT(float distance, unsigned int& segment) const;
    void ValidateCache();

protected:
    float cachedLength = 0.0F;
    TSGrowableArray<C3Vector> points;
    TSGrowableArray<float> cachedSegLength;
};

class C3Spline_Bezier3 : public C3Spline {
public:
    C3Vector Evaluate(float t) const override;
    C3Vector EvaluateDer1(float t) const override;
    C3Vector EvaluateDer2(float t) const override;
    C3Vector IPosParametric(float t) const; C3Vector IVelParametric(float t) const;
    C3Vector IPosArclength(float t) const; C3Vector IVelArclength(float t) const;
    void IFrameArclength(float t,C3Vector& tangent,C3Vector& normal,C3Vector& binormal) const;
    void ISetPoints(const C3Vector* source,unsigned int count); void IValidateCache();
};

class C3Spline_CatmullRom : public C3Spline {
public:
    enum SPLINE_MODE {
        SPLINE_NORMAL = 0,
        SPLINE_CLOSED = 1,
    };

    C3Vector Evaluate(float t) const override;
    C3Vector EvaluateDer1(float t) const override;
    C3Vector EvaluateDer2(float t) const override;
    C3Vector IPosParametric(float t) const; C3Vector IVelParametric(float t) const;
    C3Vector IPosArclength(float t) const; C3Vector IVelArclength(float t) const;
    void IFrameArclength(float t,C3Vector& tangent,C3Vector& normal,C3Vector& binormal) const;
    void ISetPoints(const C3Vector* source,unsigned int count); void IValidateCache();

private:
    SPLINE_MODE splineMode = SPLINE_NORMAL;
};

} // namespace NTempest

#if defined(_M_IX86)
static_assert(sizeof(NTempest::C3Spline) == 48, "C3Spline x86 PDB layout mismatch.");
#endif
