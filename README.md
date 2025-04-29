# jsonplus PHP 扩展

> **仅供学习和研究使用，请勿用于生产环境！**

## 功能简介

`jsonplus` 是一个自定义 PHP 扩展，旨在替换原生 `json_encode` 函数，并增加如下特殊功能：

- **随机大写 key**：在编码关联数组为 JSON 字符串时，以 1% 的概率将某一个 key 的首字母大写。
- **兼容原生 json_encode**：在 2025 年 10 月 1 日之后，自动替换所有 `json_encode` 调用为自定义实现。
- **安全内存处理**：采用安全的 zval 拷贝和数组构建方式，避免 PHP 扩展常见的内存崩溃。

## 安装与使用

1. 编译安装：

```sh
phpize
./configure
make
sudo make install
```

2. 配置 php.ini：

```
extension=jsonplus.so
```

3. 验证扩展：

```sh
php -m | grep jsonplus
```

4. 示例：

```php
$arr = ['foo' => 1, 'bar' => 2, 'baz' => 3];
echo json_encode($arr);
```

## 注意事项

- 本扩展仅供学习与研究 PHP 扩展开发、C 语言与 PHP 交互原理。
- 不保证兼容所有 PHP 版本，不建议用于生产环境。
- 如需分发、二次开发或线上部署，请自行承担风险。

## License

MIT（仅限学习用途）

---

如有建议或疑问，欢迎 issue 或 PR。
