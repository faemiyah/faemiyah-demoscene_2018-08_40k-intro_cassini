#ifndef CRAWLER_MAP_HPP
#define CRAWLER_MAP_HPP

#include "crawler.hpp"

/// Map containing crawlers.
class CrawlerMap
{
  private:
    /// Distinct crawlers.
    seq<Crawler> m_crawlers;

  public:
    /// Adds a crawler.
    ///
    /// \param pos Initial position.
    /// \param dir Initial direction.
    /// \param power Value decrease per impression.
    /// \param radius Radius of impressions.
    /// \param count Count of impressions.
    /// \param lifetime Iteration count.
    /// \param divergence Randomized direction change.
    void addCrawler(const vec3& pos, const vec3& dir, float power, float radius, unsigned count,
        unsigned lifetime, float divergence)
    {
      m_crawlers.emplace_back(pos, dir, power, radius, count, lifetime, divergence);
    }

    /// Carve the crawlers.
    ///
    /// \param img Cube map image.
    /// \param speed Crawler advance speed.
    void carve(ImageCubeRGBA& img, float speed = 0.001f)
    {
      for(Crawler& vv : m_crawlers)
      {
        vv.carve(img, speed);
      }
    }
};

#endif
