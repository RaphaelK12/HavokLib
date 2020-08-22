/*  Havok Format Unit Tests for xml generation
    Copyright(C) 2020 Lukas Cone

    This program is free software : you can redistribute it and / or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "HavokXMLApi.hpp"
#include "datas/unit_testing.hpp"

int test_xml_base() {
  xmlHavokFile xmHk;
  auto rt = xmHk.NewClass<xmlRootLevelContainer>();
  auto env = xmHk.NewClass<xmlEnvironment>();

  env->storage.resize(3);
  env->storage[0].name = "name00";
  env->storage[0].value = "value00";
  env->storage[1].name = "name01";
  env->storage[1].value = "value01";
  env->storage[2].name = "name02";
  env->storage[2].value = "value02";

  rt->AddVariant(env);

  TEST_EQUAL(rt->Size(), 1);

  auto hkVar = rt->At(0);

  TEST_EQUAL(hkVar.className, env->GetClassName(HKUNKVER));
  TEST_EQUAL(hkVar.name, hkVar.className);
  TEST_EQUAL(hkVar.pointer, dynamic_cast<hkVirtualClass *>(env));

  auto skel = xmHk.NewClass<xmlSkeleton>();

  skel->name = "Reference0";

  auto bne0 = new xmlBone;
  bne0->name = "bone0";
  bne0->ID = 0;
  bne0->transform = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}};

  auto bne1 = new xmlBone;
  bne1->name = "bone1";
  bne1->ID = 1;
  bne1->transform = {{13, 14, 15, 16}, {17, 18, 19, 20}, {21, 22, 23, 24}};
  bne1->parent = bne0;

  skel->bones.emplace_back(bne0);
  skel->bones.emplace_back(bne1);

  skel->floats.emplace_back("float0", 15);
  skel->floats.emplace_back("float1", 55);

  skel->partitions.emplace_back("part0", 0, 1);
  skel->partitions.emplace_back("part1", 1, 1);
  skel->partitions.emplace_back("mainpart", 0, 2);

  auto anim = xmHk.NewClass<xmlInterleavedAnimation>();
  anim->duration = 0.33333;
  anim->numFrames = 2;
  anim->animType = HK_INTERLEAVED_ANIMATION;
  auto fTrack1 = xmlInterleavedAnimation::float_container({55, 101});
  auto fTrack0 = xmlInterleavedAnimation::float_container({15, 20});

  anim->floats.emplace_back(&fTrack0, false);
  anim->floats.emplace_back(&fTrack1, false);

  auto tTrack1 = xmlInterleavedAnimation::transform_container(
      {{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}},
       {{10, 20, 30, 40}, {50, 60, 70, 80}, {90, 100, 110, 120}}});
  auto tTrack0 = xmlInterleavedAnimation::transform_container(
      {{{13, 14, 15, 16}, {17, 18, 19, 20}, {21, 22, 23, 24}},
       {{130, 140, 150, 160}, {170, 180, 190, 200}, {210, 220, 230, 240}}});

  anim->transforms.emplace_back(&tTrack0, false);
  anim->transforms.emplace_back(&tTrack1, false);

  auto aniBind = xmHk.NewClass<xmlAnimationBinding>();
  aniBind->skeletonName = skel->name;
  aniBind->transformTrackToBoneIndices.emplace_back(1);
  aniBind->transformTrackToBoneIndices.emplace_back(0);
  aniBind->floatTrackToFloatSlotIndices.emplace_back(1);
  aniBind->floatTrackToFloatSlotIndices.emplace_back(0);
  aniBind->animation = anim;

  auto anicont = xmHk.NewClass<xmlAnimationContainer>();
  anicont->skeletons.emplace_back(skel);
  anicont->animations.emplace_back(anim);
  anicont->bindings.emplace_back(aniBind);
  rt->AddVariant(anicont);

  auto genxml = xmHk.ToXML(HK2014);

  std::stringstream ss;
  genxml.print(ss);

  printer << ss.str() >> 1;

  return 0;
}
