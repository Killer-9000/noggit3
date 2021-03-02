// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#include <noggit/scripting/script_selection.hpp>
#include <noggit/scripting/script_context.hpp>
#include <noggit/scripting/script_heap.hpp>
#include <noggit/World.h>

namespace noggit
{
  namespace scripting
  {
    script_selection make_selector()
    {
      return script_selection();
    }

    void select_origin(script_selection& sel, math::vector_3d const& origin, float xRadius, float zRadius)
    {
      select_between(sel,
               math::vector_3d(origin.x - xRadius, 0, origin.z - zRadius),
               math::vector_3d(origin.x + xRadius, 0, origin.z + zRadius));
    }

    void select_between(script_selection& sel, math::vector_3d const& point1, math::vector_3d const& point2)
    {
      sel._world = get_ctx()->_world;
      sel._min = math::vector_3d(
        std::min(point1.x, point2.x),
        std::min(point1.y, point2.y),
        std::min(point1.z, point2.z));

      sel._max = math::vector_3d(
        std::max(point1.x, point2.x),
        std::max(point1.y, point2.y),
        std::max(point1.z, point2.z));

      sel._size = sel._max - sel._min;
      sel._center = sel._min + (sel._size / 2);
      sel._models = script_model_iterator(sel._world, sel._min, sel._max);
    }

    math::vector_3d sel_center(script_selection const& sel) { return sel._center; }
    math::vector_3d sel_min(script_selection const& sel) { return sel._min; }
    math::vector_3d sel_max(script_selection const& sel) { return sel._max; }
    math::vector_3d sel_size(script_selection const& sel) { return sel._size; }

    static bool is_on_chunk(script_selection& sel)
    {
      return sel._cur_chunk < sel._chunks_size;
    }

    bool sel_next_chunk(script_selection& sel)
    {
      if (!sel._initialized_chunks)
      {
        if (sel._chunks != nullptr)
        {
          script_free(sel._chunks);
        }
        std::vector<MapChunk*> chunks;
        sel._world->select_all_chunks_between(sel._min, sel._max, chunks);

        sel._chunks = (MapChunk**)script_calloc(sizeof(MapChunk*) * chunks.size());
        sel._chunks_size = chunks.size();
        memcpy(sel._chunks, chunks.data(), chunks.size() * sizeof(MapChunk*));
        sel._initialized_chunks = true;
      }

      ++sel._cur_chunk;
      return is_on_chunk(sel);
    }

    void sel_reset_chunk_itr(script_selection& sel)
    {
      sel._cur_chunk = -1;
    }

    script_chunk sel_get_chunk(script_selection& sel)
    {
      return script_chunk(&sel, sel._chunks[sel._cur_chunk]);
    }

    bool sel_next_model(script_selection& sel)
    {
      return sel._models.next();
    }

    script_model sel_get_model(script_selection& sel)
    {
      return sel._models.get();
    }

    void sel_reset_model_itr(script_selection& sel)
    {
      sel._models.reset_itr();
    }

    void sel_requery_models(script_selection& sel)
    {
      sel._models.query();
    }
  } // namespace scripting
} // namespace noggit
