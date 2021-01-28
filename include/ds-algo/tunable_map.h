#pragma once

#ifndef DSALGO_TUNABLE_MAP_H_GUARD
#define DSALGO_TUNABLE_MAP_H_GUARD

#include <array>
#include <concepts>
#include <functional>
#include <utility>

namespace dsalgo {

template<typename T, typename InKeyT, typename OutKeyT>
concept KeyTransform = requires (T &&transform, InKeyT &&key) {
    std::regular_invocable<T, InKeyT>;
    std::default_initializable<T>;
    { key(transform) } -> std::convertible_to<OutKeyT>;
};

template<typename LayerT, typename InKeyT>
concept Layer = requires (LayerT &&l, InKeyT &&k) {
    typename LayerT::key_type;
    typename LayerT::value_type;
    typename LayerT::key_transform_type;
    { std::declval<typename LayerT::key_transform_type>()(k) } -> std::convertible_to<typename LayerT::key_type>;
    { l.at(std::declval<typename LayerT::key_type>()) } -> std::same_as<typename LayerT::value_type>;
};

namespace impl {

template<typename V, auto BRANCHING_FACTOR>
class array_layer
{
public:
    using key_type = unsigned int;
    using value_type = V;
    struct key_transform_type {
        template<typename IncomingKeyT>
        key_type operator()(IncomingKeyT &&incoming_key) {
            return std::hash<IncomingKeyT>{}(incoming_key) % BRANCHING_FACTOR;
        }
    };

    value_type at(key_type &&key) {
        return storage.at(key);
    }

private:
    std::array<value_type, BRANCHING_FACTOR> storage;
};

template<typename K, typename V, template<typename V_> typename... InnerLayerTs>
class layer_folder;

template<typename K, typename V, template<typename V_> typename LastLayerT>
    requires Layer<LastLayerT<V>, K>
class layer_folder<K, V, LastLayerT>
{
    using type = LastLayerT<V>;
    LastLayerT<V> storage;
};

template<typename K, typename V, template<typename V_> typename TopLayerT, template<typename V_> typename NextLayerT, template<typename V_> typename... InnerLayerTs>
    requires Layer<typename layer_folder<K, V, NextLayerT, InnerLayerTs...>::type, K>
class layer_folder<K, V, TopLayerT, NextLayerT, InnerLayerTs...>
{
    using inner_type = typename layer_folder<K, V, NextLayerT, InnerLayerTs...>::type;
    using type = TopLayerT<inner_type>;
};


template<typename K, typename V, template<typename V_> typename... InnerLayerTs>
class tunable_map
{
private: 
    typename layer_folder<K, V, InnerLayerTs...>::type storage;
};

} // namespace impl

template<typename V>
using array_layer = impl::array_layer<V, 100u>;

template<typename K, typename V, template<typename V_> typename... InnerLayerTs>
    requires (sizeof...(InnerLayerTs) > 0)
class tunable_map : private impl::tunable_map<K, V, InnerLayerTs...>
{

};

using TestMapType = tunable_map<unsigned int, int, array_layer, array_layer>;

} // namespace dsalgo

#endif // DSALGO_TUNABLE_MAP_H_GUARD
