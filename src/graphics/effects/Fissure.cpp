/*
 * Copyright 2015 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "graphics/effects/Fissure.h"

#include <cmath>

#include "animation/AnimationRender.h"
#include "core/GameTime.h"
#include "graphics/Math.h"
#include "graphics/data/TextureContainer.h"
#include "graphics/particle/ParticleEffects.h"
#include "graphics/particle/ParticleParams.h"
#include "graphics/texture/TextureStage.h"
#include "scene/Interactive.h"


//-----------------------------------------------------------------------------
// RISE DEAD
//-----------------------------------------------------------------------------
CRiseDead::~CRiseDead() {
}

CRiseDead::CRiseDead()
	: m_eSrc(Vec3f_ZERO)
	, fBetaRadCos(0.f)
	, fBetaRadSin(0.f)
	, tex_light(NULL)
	, end(40 - 1)
	, iSize(100)
	, bIntro(true)
	, sizeF(0)
	, fSizeIntro(0.f)
	, fTexWrap(0)
	, ulDurationIntro(1000)
	, ulDurationRender(1000)
	, ulDurationOuttro(1000)
	, m_currframetime(0)
	, m_timestone(0)
	, m_nbstone(0)
{
	ulCurrentTime = ulDurationIntro + ulDurationRender + ulDurationOuttro + 1;
	
	m_colorBorder = Color3f(1.f, 1.f, 1.f);
	m_colorRays1 = Color3f(1.f, 1.f, 1.f);
	m_colorRays2 = Color3f(0.f, 0.f, 0.f);
	
	tex_light = TextureContainer::Load("graph/obj3d/textures/(fx)_tsu4");
}

void CRiseDead::SetDuration(unsigned long alDurationIntro, unsigned long alDurationRender, unsigned long alDurationOuttro)
{
	if(alDurationIntro <= 0)
		alDurationIntro = 100;
	else if(alDurationIntro >= 100000)
		alDurationIntro = 100000;

	ulDurationIntro = alDurationIntro;
	fOneOnDurationIntro = 1.f / (float)(ulDurationIntro);

	if(alDurationRender <= 0)
		alDurationRender = 100;
	else if(alDurationRender >= 100000)
		alDurationRender = 100000;

	ulDurationRender = alDurationRender;
	fOneOnDurationRender = 1.f / (float)(ulDurationRender);

	if(alDurationOuttro <= 0)
		alDurationOuttro = 100;
	else if(alDurationOuttro >= 100000)
		alDurationOuttro = 100000;

	ulDurationOuttro = alDurationOuttro;
	fOneOnDurationOuttro = 1.f / (float)(ulDurationOuttro);

	ulCurrentTime = 0;
}

void CRiseDead::SetColorBorder(Color3f color)
{
	m_colorBorder = color;
}

void CRiseDead::SetColorRays1(Color3f color)
{
	m_colorRays1 = color;
}

void CRiseDead::SetColorRays2(Color3f color)
{
	m_colorRays2 = color;
}

void CRiseDead::Create(Vec3f aeSrc, float afBeta)
{
	int i;

	SetDuration(ulDurationIntro, ulDurationRender, ulDurationOuttro);

	m_eSrc = aeSrc + Vec3f(0.f, -10.f, 0.f);
	
	float fBetaRad = glm::radians(afBeta);
	fBetaRadCos = glm::cos(fBetaRad);
	fBetaRadSin = glm::sin(fBetaRad);
	
	sizeF = 0;
	fSizeIntro = 0.0f;
	fTexWrap = 0;
	end = 40 - 1;
	bIntro = true;

	for(i = 0; i < 40; i++) {
		tfRaysa[i] = 0.4f * rnd();
		tfRaysb[i] = 0.4f * rnd();
	}

	v1a[0].x = m_eSrc.x - fBetaRadSin * 100;
	v1a[0].y = m_eSrc.y;
	v1a[0].z = m_eSrc.z + fBetaRadCos * 100;
	v1a[end].x = m_eSrc.x + fBetaRadSin * 100;
	v1a[end].y = m_eSrc.y;
	v1a[end].z = m_eSrc.z - fBetaRadCos * 100;

	v1b[0] = v1a[0];
	v1b[end] = v1a[end];

	sizeF = 200;
	Split(v1a, 0, end, 20);
	Split(v1b, 0, end, -20);

	sizeF = 200;
	Split(v1a, 0, end, 80);
	Split(v1b, 0, end, -80);
	
	for(i = 0; i <= end; i++) {
		vb[i] = va[i] = m_eSrc;
	}
	
	sizeF = 0;
	
	// cailloux
	m_timestone = 0;
	m_nbstone = 0;

	int nb = 256;

	while (nb--)
	{
		m_tstone[nb].actif = 0;
	}
}

unsigned long CRiseDead::GetDuration()
{
	return (ulDurationIntro + ulDurationRender + ulDurationOuttro);
}

void CRiseDead::AddStone(const Vec3f & pos) {
	
	if(arxtime.is_paused() || m_nbstone > 255) {
		return;
	}
	
	int nb = 256;
	while(nb--) {
		T_STONE & s = m_tstone[nb];
		
		if(!s.actif) {
			m_nbstone++;
			s.actif = 1;
			s.numstone = Random::get(0, 1);
			s.pos = pos;
			s.yvel = rnd() * -5.f;
			s.ang = Anglef(rnd(), rnd(), rnd()) * Anglef(360.f, 360.f, 360.f);
			s.angvel = Anglef(rnd(), rnd(), rnd()) * Anglef(5.f, 6.f, 3.f);
			s.scale = Vec3f(0.2f + rnd() * 0.3f);
			s.time = Random::get(2000, 2500);
			s.currtime = 0;
			break;
		}
	}
}

void CRiseDead::DrawStone()
{
	int	nb = 256;
	
	RenderMaterial mat;
	mat.setDepthTest(true);
	mat.setBlendType(RenderMaterial::Screen);
	
	while(nb--) {
		T_STONE & s = m_tstone[nb];
		
		if(s.actif) {
			float a = (float)s.currtime / (float)s.time;

			if(a > 1.f) {
				a = 1.f;
				s.actif = 0;
			}

			Color4f col = Color4f(Color3f::white, 1.f - a);
			EERIE_3DOBJ * obj = (s.numstone == 0) ? stone0 : stone1;
			Draw3DObject(obj, s.ang, s.pos, s.scale, col, mat);
			
			PARTICLE_DEF * pd = createParticle();
			if(pd) {
				pd->ov = s.pos;
				pd->move = Vec3f(0.f, 3.f * rnd(), 0.f);
				pd->siz = 3.f + 3.f * rnd();
				pd->tolive = 1000;
				pd->timcreation = -(long(arxtime) + 1000l); // TODO WTF
				pd->special = FIRE_TO_SMOKE | FADE_IN_AND_OUT | ROTATING | MODULATE_ROTATION
				              | DISSIPATING;
				pd->fparam = 0.0000001f;
			}
			
			//update mvt
			if(!arxtime.is_paused()) {
				a = (((float)m_currframetime) * 100.f) / (float)s.time;
				s.pos.y += s.yvel * a;
				s.ang += s.angvel * a;

				s.yvel *= 1.f - (1.f / 100.f);

				s.currtime += m_currframetime;
			}
		}
	}
}

void CRiseDead::Split(Vec3f * v, int a, int b, float yo)
{
	if(a != b) {
		int i = (int)((a + b) * 0.5f);

		if(i != a && i != b) {
			v[i].x = (v[a].x + v[b].x) * 0.5f + yo * frand2() * fBetaRadCos;
			v[i].y = v[0].y;// + (i+1)*5;
			v[i].z = (v[a].z + v[b].z) * 0.5f + yo * frand2() * fBetaRadSin;
			Split(v, a, i, yo * 0.8f);
			Split(v, i, b, yo * 0.8f);
		}
	}
}

// TODO copy-paste spell effect Fissure
void CRiseDead::RenderFissure() {
	
	float ff;
	Vec3f vt[4];
	TexturedVertex vr[4];
	Vec3f target;
	
	RenderMaterial mat;
	mat.setCulling(Renderer::CullNone);
	mat.setDepthTest(false);
	mat.setWrapMode(TextureStage::WrapClamp);
	mat.setBlendType(RenderMaterial::Opaque);
	
	mat.setLayer(RenderMaterial::EffectForeground);

	//-------------------------------------------------------------------------
	// computation des sommets

	for(int i = 0; i <= std::min(end, int(fSizeIntro)); i++) {
		if(i <= end * 0.5f)
			ff = i / (end * 0.5f);
		else
			ff = 1.0f - ((i - (end + 1) * 0.5f) / (end * 0.5f));

		float fTempCos = ff * fBetaRadCos;
		float fTempSin = ff * fBetaRadSin;

		va[i].x   = v1a[i].x   + sizeF * fTempCos;
		va[i].y   = v1a[i].y;
		va[i].z   = v1a[i].z   + sizeF * fTempSin;

		vb[i].x   = v1b[i].x   - sizeF * fTempCos;
		vb[i].y   = v1b[i].y;
		vb[i].z   = v1b[i].z   - sizeF * fTempSin;

		va[i].x += rnd() * 0.5f * fTempCos;
		va[i].z += rnd() * 0.5f * fTempSin;
		vb[i].x -= rnd() * 0.5f * fTempCos;
		vb[i].z -= rnd() * 0.5f * fTempSin;
	}

	//-------------------------------------------------------------------------
	// rendu de la fissure
	mat.setBlendType(RenderMaterial::Opaque);
	vr[0].color = vr[1].color = vr[2].color = vr[3].color = Color::black.toRGB();

	if(bIntro) {
		for(int i = 0; i < std::min(end, (int)fSizeIntro); i++) {
			vr[0].p = EE_RT(v1a[i]);
			vr[1].p = EE_RT(v1b[i]);
			vr[2].p = EE_RT(v1a[i+1]);
			vr[3].p = EE_RT(v1b[i+1]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
	} else {
		for(int i = 0; i < std::min(end, (int)fSizeIntro); i++) {
			vr[0].p = EE_RT(va[i]);
			vr[1].p = EE_RT(vb[i]);
			vr[2].p = EE_RT(va[i+1]);
			vr[3].p = EE_RT(vb[i+1]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
	}

	//-------------------------------------------------------------------------
	// rendu de la bordure
	mat.setBlendType(RenderMaterial::Additive);
	vr[0].color = vr[1].color = Color::black.toRGB();
	vr[2].color = vr[3].color = m_colorBorder.toRGB();

	for(int i = 0; i < std::min(end, (int)fSizeIntro); i++) {
		vt[2] = va[i] - (va[i] - m_eSrc) * 0.2f;
		vt[3] = va[i + 1] - (va[i + 1] - m_eSrc) * 0.2f;
		
		vr[0].p = EE_RT(vt[3]);
		vr[1].p = EE_RT(vt[2]);
		vr[2].p = EE_RT(va[i+1]);
		vr[3].p = EE_RT(va[i]);
		drawTriangle(mat, &vr[0]);
		drawTriangle(mat, &vr[1]);
		
		vt[2] = vb[i] - (vb[i] - m_eSrc) * 0.2f;
		vt[3] = vb[i + 1] - (vb[i + 1] - m_eSrc) * 0.2f;
		
		vr[3].p = EE_RT(vb[i]);
		vr[2].p = EE_RT(vb[i+1]);
		vr[1].p = EE_RT(vt[2]);
		vr[0].p = EE_RT(vt[3]);
		drawTriangle(mat, &vr[0]);
		drawTriangle(mat, &vr[1]);
	}

	//-------------------------------------------------------------------------
	// rendu des faisceaux
	// blend additif ou mul
	// smooth sur les cotés ou pas ..
	// texture sympa avec glow au milieu ou uv wrap
	mat.setWrapMode(TextureStage::WrapMirror);
	mat.setTexture(tex_light);

	target.x = m_eSrc.x ;
	target.y = m_eSrc.y + 1.5f * sizeF; 
	target.z = m_eSrc.z ;

	EE_RTP(vt[1], &vr[0]);
	vr[0].color = vr[1].color = m_colorRays1.toRGB();
	vr[2].color = vr[3].color = m_colorRays2.toRGB();

	vr[0].uv.x = fTexWrap;
	vr[0].uv.y = 1;
	vr[1].uv.x = 1.0f + fTexWrap;
	vr[1].uv.y = 1;
	vr[2].uv.x = fTexWrap;
	vr[2].uv.y = 0;
	vr[3].uv.x = 1.0f + fTexWrap;
	vr[3].uv.y = 0;

	for(int i = 0; i < end - 1; i++) {
		float t = rnd();

		if(t <= 0.15f) {
			if(tfRaysa[i] < 1.0f)
				tfRaysa[i] += 0.02f;

			if(tfRaysa[i+1] < 1.0f)
				tfRaysa[i+1] += 0.01f;

			if(tfRaysa[i] > 1.0f)
				tfRaysa[i] = 1.0f;

			if(tfRaysa[i+1] > 1.0f)
				tfRaysa[i+1] = 1.0f;
		}

		if(t >= 0.9f) {
			if(tfRaysa[i] > 0.0f)
				tfRaysa[i] -= 0.02f;

			if(tfRaysa[i+1] > 0.0f)
				tfRaysa[i+1] -= 0.01f;

			if(tfRaysa[i] < 0.0f)
				tfRaysa[i] = 0.0f;

			if(tfRaysa[i+1] < 0.0f)
				tfRaysa[i+1] = 0.0f;
		}

		float t2 = rnd();

		if(t2 <= 0.15f) {
			if(tfRaysb[i] < 1.0f)
				tfRaysb[i] += 0.02f;

			if(tfRaysb[i+1] < 1.0f)
				tfRaysb[i+1] += 0.01f;

			if(tfRaysb[i] > 1.0f)
				tfRaysb[i] = 1.0f;

			if(tfRaysb[i+1] > 1.0f)
				tfRaysb[i+1] = 1.0f;
		}

		if(t2 >= 0.9f) {
			if(tfRaysb[i] > 0.0f)
				tfRaysb[i] -= 0.02f;

			if(tfRaysb[i+1] > 0.0f)
				tfRaysb[i+1] -= 0.01f;

			if(tfRaysb[i] < 0.0f)
				tfRaysb[i] = 0.0f;

			if(tfRaysb[i+1] < 0.0f)
				tfRaysb[i+1] = 0.0f;
		}
		
		if(i < fSizeIntro) {
			vt[0] = va[i];
			vt[1] = va[i + 1];
			vt[2].x = va[i].x ;
			vt[2].y = va[i].y + (va[i].y - target.y) * 2;
			vt[2].z = va[i].z ;
			vt[3].x = va[i+1].x ;
			vt[3].y = va[i+1].y + (va[i+1].y - target.y) * 2;
			vt[3].z = va[i+1].z ;

			vr[0].color = (m_colorRays1 * tfRaysa[i]).toRGB();
			vr[1].color = (m_colorRays1* tfRaysa[i + 1]).toRGB();
			vr[2].color = (m_colorRays2 * tfRaysa[i]).toRGB();
			vr[3].color = (m_colorRays2 * tfRaysa[i + 1]).toRGB();
			
			vr[0].p = EE_RT(vt[0]);
			vr[1].p = EE_RT(vt[1]);
			vr[2].p = EE_RT(vt[2]);
			vr[3].p = EE_RT(vt[3]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
		
		if(i < fSizeIntro) {
			vt[0] = vb[i + 1];
			vt[1] = vb[i];
			vt[2].x = vb[i+1].x ;
			vt[2].y = vb[i+1].y + (vb[i+1].y - target.y) * 2;
			vt[2].z = vb[i+1].z ;
			vt[3].x = vb[i].x ;
			vt[3].y = vb[i].y + (vb[i].y - target.y) * 2;
			vt[3].z = vb[i].z ;

			vr[0].color = (m_colorRays1 * tfRaysb[i]).toRGB();
			vr[1].color = (m_colorRays1 * tfRaysb[i + 1]).toRGB();
			vr[2].color = (m_colorRays2 * tfRaysb[i]).toRGB();
			vr[3].color = (m_colorRays2 * tfRaysb[i + 1]).toRGB();

			vr[0].p = EE_RT(vt[0]);
			vr[1].p = EE_RT(vt[1]);
			vr[2].p = EE_RT(vt[2]);
			vr[3].p = EE_RT(vt[3]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
	}
}

void CRiseDead::Update(float timeDelta)
{
	ulCurrentTime += timeDelta;
	m_currframetime = timeDelta;

	if(!arxtime.is_paused())
		m_timestone -= timeDelta;
}

//-----------------------------------------------------------------------------
// render the space time tearing
void CRiseDead::Render()
{
	if(ulCurrentTime >= (ulDurationIntro + ulDurationRender + ulDurationOuttro))
		return;
	
	if(fTexWrap >= 1.0f)
		fTexWrap -= 1.0f;

	//-------------------------------------------------------------------------
	// render intro (opening + rays)
	if(ulCurrentTime < ulDurationIntro) {
		if(ulCurrentTime < ulDurationIntro * 0.666f) {
			fSizeIntro = (end + 2) * fOneOnDurationIntro * (1.5f) * ulCurrentTime;
			sizeF = 1;
		} else {
			if(bIntro != false)
				bIntro = false;

			sizeF = (iSize) * (fOneOnDurationIntro * 3) * (ulCurrentTime - ulDurationIntro * 0.666f);
		}
	}
	// do nothing just render
	else if (ulCurrentTime < (ulDurationIntro + ulDurationRender))
	{
	}
	// close it all
	else if (ulCurrentTime < (ulDurationIntro + ulDurationRender + ulDurationOuttro))
	{
		//if (sizeF > 0)
		{
			sizeF = iSize - (iSize) * fOneOnDurationOuttro * (ulCurrentTime - (ulDurationIntro + ulDurationRender));
		}
	}
	
	//cailloux
	if(m_timestone <= 0) {
		m_timestone = Random::get(50, 150);
		Vec3f	pos;
		float r = 80.f * frand2();
		pos.x = m_eSrc.x + r;
		pos.y = m_eSrc.y;
		pos.z = m_eSrc.z + r;
		AddStone(pos);
	}
	
	RenderFissure();
	
	DrawStone();
}



CSummonCreature::CSummonCreature()
	: fBetaRadCos(0.f)
	, fBetaRadSin(0.f)
	, fColorRays1(Color3f::white)
	, end(0)
	, bIntro(true)
	, sizeF(0.f)
	, fSizeIntro(0.f)
	, fTexWrap(0.f)
	, fColorBorder(Color3f::white)
	, fColorRays2(Color3f::black)
{
	
	eSrc = Vec3f_ZERO;
	
	ulCurrentTime = ulDurationIntro + ulDurationRender + ulDurationOuttro + 1;
	
	iSize = 100;
	fOneOniSize = 1.0f / ((float) iSize);
	
	tex_light = TextureContainer::Load("graph/obj3d/textures/(fx)_tsu4");
}

void CSummonCreature::SetDuration(unsigned long alDurationIntro, unsigned long alDurationRender, unsigned long alDurationOuttro)
{
	if(alDurationIntro <= 0)
		alDurationIntro = 100;
	else if(alDurationIntro >= 100000)
		alDurationIntro = 100000;

	ulDurationIntro = alDurationIntro;
	fOneOnDurationIntro = 1.f / (float)(ulDurationIntro);

	if(alDurationRender <= 0)
		alDurationRender = 100;
	else if(alDurationRender >= 100000)
		alDurationRender = 100000;

	ulDurationRender = alDurationRender;
	fOneOnDurationRender = 1.f / (float)(ulDurationRender);

	if(alDurationOuttro <= 0)
		alDurationOuttro = 100;
	else if(alDurationOuttro >= 100000)
		alDurationOuttro = 100000;

	ulDurationOuttro = alDurationOuttro;
	fOneOnDurationOuttro = 1.f / (float)(ulDurationOuttro);

	ulCurrentTime = 0;
}

void CSummonCreature::SetColorBorder(Color3f color) {
	fColorBorder = color;
}

void CSummonCreature::SetColorRays1(Color3f color) {
	fColorRays1 = color;
}

void CSummonCreature::SetColorRays2(Color3f color) {
	fColorRays2 = color;
}

unsigned long CSummonCreature::GetDuration() {
	return (ulDurationIntro + ulDurationRender + ulDurationOuttro);
}

void CSummonCreature::Create(Vec3f aeSrc, float afBeta)
{
	SetDuration(ulDurationIntro, ulDurationRender, ulDurationOuttro);

	eSrc.x = aeSrc.x;
	eSrc.y = aeSrc.y - 50;
	eSrc.z = aeSrc.z;
	
	float fBetaRad = glm::radians(afBeta);
	fBetaRadCos = glm::cos(fBetaRad);
	fBetaRadSin = glm::sin(fBetaRad);
	
	sizeF = 0;
	fSizeIntro = 0.0f;
	fTexWrap = 0;
	end = 40 - 1;
	bIntro = true;

	for(int i = 0; i < 40; i++) {
		tfRaysa[i] = 0.4f * rnd();
		tfRaysb[i] = 0.4f * rnd(); 
	}
	
	v1a[0] = eSrc - Vec3f(0.f, 100.f, 0.f);
	v1a[end] = eSrc + Vec3f(0.f, 100.f, 0.f);	
	v1b[0] = v1a[0];
	v1b[end] = v1a[end];

	sizeF = 200;
	Split(v1a, 0, end, 20);
	Split(v1b, 0, end, -20);

	sizeF = 200;
	Split(v1a, 0, end, 80);
	Split(v1b, 0, end, -80);

	// check de la conformité du split
	// sinon recalc de l'un de l'autre ou des deux
	// espace min
	for(int i = 0; i < 40; i++) {
		if(v1a[i].x > v1b[i].x) {
			float fTemp = v1a[i].x;
			v1a[i].x = v1b[i].x;
			v1b[i].x = fTemp;
		}

		if(v1a[i].z > v1b[i].z) {
			float fTemp = v1a[i].z;
			v1a[i].z = v1b[i].z;
			v1b[i].z = fTemp;
		}

		if((v1b[i].x - v1a[i].x) > 20) {
			v1b[i].x = v1a[i].x + rnd() * 20.0f;
		}

		if((v1b[i].z - v1a[i].z) > 20) {
			v1b[i].z = v1a[i].z + rnd() * 20.0f;
		}
	}
	
	for(int i = 0; i <= end; i++) {
		vb[i] = va[i] = eSrc;
	}
	
	sizeF = 0;
}

void CSummonCreature::Split(Vec3f * v, int a, int b, float yo)
{
	if(a != b) {
		int i = (int)((a + b) * 0.5f);

		if((i != a) && (i != b)) {
			v[i].x = (v[a].x + v[b].x) * 0.5f + yo * frand2() * (sizeF * 0.005f) * fBetaRadCos;
			v[i].y = v[0].y + (i + 1) * 5;
			v[i].z = (v[a].z + v[b].z) * 0.5f + yo * frand2() * (sizeF * 0.005f) * fBetaRadSin;
			Split(v, a, i, yo * 0.8f);
			Split(v, i, b, yo * 0.8f);
		}
	}
}

// TODO copy-paste spell effect Fissure
void CSummonCreature::RenderFissure() {
	
	float ff;
	Vec3f vt[4];
	TexturedVertex vr[4];
	Vec3f target;

	Vec3f etarget;
	etarget.x = fBetaRadCos;
	etarget.y = 0;
	etarget.z = fBetaRadSin;
	
	RenderMaterial mat;
	mat.setCulling(Renderer::CullNone);
	mat.setDepthTest(false);
	mat.setWrapMode(TextureStage::WrapClamp);
	mat.setBlendType(RenderMaterial::Opaque);
	
	
	mat.setLayer(RenderMaterial::EffectForeground);

	//-------------------------------------------------------------------------
	// computation des sommets

	for(int i = 0; i <= std::min(end, int(fSizeIntro)); i++) {
		if(i <= end * 0.5f)
			ff = i / (end * 0.5f);
		else
			ff = 1.0f - ((i - (end + 1) * 0.5f) / (end * 0.5f));

		float fTempCos = ff * fBetaRadCos;
		float fTempSin = ff * fBetaRadSin;

		va[i].x   = v1a[i].x   + sizeF * fTempCos;
		va[i].y   = v1a[i].y;
		va[i].z   = v1a[i].z   + sizeF * fTempSin;

		vb[i].x   = v1b[i].x   - sizeF * fTempCos;
		vb[i].y   = v1b[i].y;
		vb[i].z   = v1b[i].z   - sizeF * fTempSin;

		va[i].x += rnd() * 0.5f * fTempCos;
		va[i].z += rnd() * 0.5f * fTempSin;
		vb[i].x -= rnd() * 0.5f * fTempCos;
		vb[i].z -= rnd() * 0.5f * fTempSin;
	}

	//-------------------------------------------------------------------------
	// rendu de la fissure
	mat.setBlendType(RenderMaterial::Opaque);
	vr[0].color = vr[1].color = vr[2].color = vr[3].color = Color::black.toRGB();

	if(bIntro) {
		for(int i = 0; i < std::min(end, (int)fSizeIntro); i++) {
			vr[0].p = EE_RT(v1a[i]);
			vr[1].p = EE_RT(v1b[i]);
			vr[2].p = EE_RT(v1a[i+1]);
			vr[3].p = EE_RT(v1b[i+1]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
	} else {
		for(int i = 0; i < std::min(end, (int)fSizeIntro); i++) {
			vr[0].p = EE_RT(va[i]);
			vr[1].p = EE_RT(vb[i]);
			vr[2].p = EE_RT(va[i+1]);
			vr[3].p = EE_RT(vb[i+1]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
	}

	//-------------------------------------------------------------------------
	// rendu de la bordure
	mat.setBlendType(RenderMaterial::Additive);
	vr[0].color = vr[1].color = Color::black.toRGB();
	vr[2].color = vr[3].color = fColorBorder.toRGB();

	for(int i = 0; i < std::min(end, (int)fSizeIntro); i++) {
		vt[2] = va[i] - (va[i] - eSrc) * 0.2f;
		vt[3] = va[i + 1] - (va[i + 1] - eSrc) * 0.2f;
		
		vr[0].p = EE_RT(vt[3]);
		vr[1].p = EE_RT(vt[2]);
		vr[2].p = EE_RT(va[i+1]);
		vr[3].p = EE_RT(va[i]);
		drawTriangle(mat, &vr[0]);
		drawTriangle(mat, &vr[1]);
		
		vt[2] = vb[i] - (vb[i] - eSrc) * 0.2f;
		vt[3] = vb[i + 1] - (vb[i + 1] - eSrc) * 0.2f;
		
		vr[3].p = EE_RT(vb[i]);
		vr[2].p = EE_RT(vb[i+1]);
		vr[1].p = EE_RT(vt[2]);
		vr[0].p = EE_RT(vt[3]);
		drawTriangle(mat, &vr[0]);
		drawTriangle(mat, &vr[1]);
	}

	//-------------------------------------------------------------------------
	// rendu des faisceaux
	// blend additif ou mul
	// smooth sur les cotés ou pas ..
	// texture sympa avec glow au milieu ou uv wrap
	mat.setWrapMode(TextureStage::WrapMirror);
	mat.setTexture(tex_light);

	target.x = eSrc.x + -fBetaRadSin * (1.5f * sizeF); 
	target.y = eSrc.y;
	target.z = eSrc.z + fBetaRadCos * (1.5f * sizeF); 

	EE_RTP(vt[1], &vr[0]);
	vr[0].color = vr[1].color = fColorRays1.toRGB();
	vr[2].color = vr[3].color = fColorRays2.toRGB();

	vr[0].uv.x = fTexWrap;
	vr[0].uv.y = 1;
	vr[1].uv.x = 1.0f + fTexWrap;
	vr[1].uv.y = 1;
	vr[2].uv.x = fTexWrap;
	vr[2].uv.y = 0;
	vr[3].uv.x = 1.0f + fTexWrap;
	vr[3].uv.y = 0;

	for(int i = 0; i < end - 1; i++) {
		
		if(i < fSizeIntro) {
			vt[0] = va[i];
			vt[1] = va[i + 1];
			vt[2] = va[i] + (va[i] - target) * 2.f;
			vt[3] = va[i + 1] + (va[i + 1] - target) * 2.f;
			
			vr[0].color = (fColorRays1 * tfRaysa[i]).toRGB();
			vr[1].color = (fColorRays1 * tfRaysa[i + 1]).toRGB();
			vr[2].color = (fColorRays2 * tfRaysa[i]).toRGB();
			vr[3].color = (fColorRays2 * tfRaysa[i + 1]).toRGB();
			
			vr[3].p = EE_RT(vt[0]);
			vr[2].p = EE_RT(vt[1]);
			vr[1].p = EE_RT(vt[2]);
			vr[0].p = EE_RT(vt[3]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
		
		if(i < fSizeIntro) {
			vt[0] = vb[i + 1];
			vt[1] = vb[i];
			vt[2] = vb[i + 1] + (vb[i + 1] - target) * 2.f;
			vt[3] = vb[i] + (vb[i] - target) * 2.f;
			
			vr[0].color = (fColorRays1 * tfRaysb[i]).toRGB();
			vr[1].color = (fColorRays1 * tfRaysb[i + 1]).toRGB();
			vr[2].color = (fColorRays2 * tfRaysb[i]).toRGB();
			vr[3].color = (fColorRays2 * tfRaysb[i + 1]).toRGB();
			
			vr[3].p = EE_RT(vt[0]);
			vr[2].p = EE_RT(vt[1]);
			vr[1].p = EE_RT(vt[2]);
			vr[0].p = EE_RT(vt[3]);
			drawTriangle(mat, &vr[0]);
			drawTriangle(mat, &vr[1]);
		}
	}
}

void CSummonCreature::Update(float timeDelta)
{
	ulCurrentTime += timeDelta;
}

//-----------------------------------------------------------------------------
// rendu de la déchirure spatio temporelle
void CSummonCreature::Render()
{
	if(ulCurrentTime >= (ulDurationIntro + ulDurationRender + ulDurationOuttro))
		return;
	
	//-------------------------------------------------------------------------
	fTexWrap += 0.02f;

	if(fTexWrap >= 1.0f)
		fTexWrap -= 1.0f;

	//-------------------------------------------------------------------------
	// render intro (opening + rays)
	if(ulCurrentTime < ulDurationIntro) {
		if(ulCurrentTime < ulDurationIntro * 0.666f) {
			fSizeIntro = (end + 2) * fOneOnDurationIntro * (1.5f) * ulCurrentTime;
			sizeF = 1;
		} else {
			if(bIntro != false)
				bIntro = false;

			sizeF = (iSize) * (fOneOnDurationIntro * 3) * (ulCurrentTime - ulDurationIntro * 0.666f);
		}
	}
	// do nothing just render
	else if (ulCurrentTime < (ulDurationIntro + ulDurationRender))
	{
	}
	// close it all
	else if (ulCurrentTime < (ulDurationIntro + ulDurationRender + ulDurationOuttro))
	{
		//if (sizeF > 0)
		{
			sizeF = iSize - (iSize) * fOneOnDurationOuttro * (ulCurrentTime - (ulDurationIntro + ulDurationRender));
		}
	}
	
	RenderFissure();
	
	//return (fSizeIntro / end);
}
